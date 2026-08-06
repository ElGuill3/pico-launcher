#include <cassert>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

#include "startupIntro/Pani.h"

namespace
{
constexpr std::size_t HeaderSize = 32;
constexpr std::size_t PaletteSize = 256 * 2;
constexpr std::size_t FrameEntrySize = 12;
constexpr std::size_t SectorSize = 512;

std::size_t RoundUpSector(std::size_t size)
{
    return (size + SectorSize - 1) & ~(SectorSize - 1);
}

void WriteLe16(std::vector<std::uint8_t>& data, std::size_t offset, std::uint16_t value)
{
    data[offset] = value;
    data[offset + 1] = value >> 8;
}

void WriteLe32(std::vector<std::uint8_t>& data, std::size_t offset, std::uint32_t value)
{
    data[offset] = value;
    data[offset + 1] = value >> 8;
    data[offset + 2] = value >> 16;
    data[offset + 3] = value >> 24;
}

std::vector<std::uint8_t> MakePackage(std::uint16_t frameCount = 1, std::uint16_t duration = 30,
    std::uint16_t codec = 0, std::uint32_t frameLength = startup_intro::PaniFrameBytes)
{
    const std::size_t tableOffset = HeaderSize + PaletteSize;
    const std::size_t tableEnd = tableOffset + frameCount * FrameEntrySize;
    const std::size_t framesOffset = codec == 1 ? RoundUpSector(tableEnd) : tableEnd;
    const std::size_t storedFrameLength = codec == 1 ? RoundUpSector(frameLength) : frameLength;
    const std::size_t fileSize = framesOffset + frameCount * storedFrameLength;
    std::vector<std::uint8_t> data(fileSize);

    data[0] = 'P';
    data[1] = 'A';
    data[2] = 'N';
    data[3] = 'I';
    WriteLe16(data, 4, 1);
    WriteLe16(data, 6, HeaderSize);
    WriteLe16(data, 8, startup_intro::PaniWidth);
    WriteLe16(data, 10, startup_intro::PaniHeight);
    WriteLe16(data, 12, frameCount);
    WriteLe16(data, 14, codec);
    WriteLe32(data, 16, HeaderSize);
    WriteLe32(data, 20, tableOffset);
    WriteLe32(data, 24, fileSize);

    for (std::uint16_t i = 0; i < frameCount; i++)
    {
        const std::size_t entry = tableOffset + i * FrameEntrySize;
        WriteLe32(data, entry, framesOffset + i * storedFrameLength);
        WriteLe32(data, entry + 4, frameLength);
        WriteLe16(data, entry + 8, duration);
    }
    return data;
}

std::vector<std::uint8_t> RepeatedByteLz4()
{
    std::vector<std::uint8_t> block{0x1f, 7, 1, 0};
    std::size_t extension = startup_intro::PaniFrameBytes - 1 - 4 - 15;
    while (extension >= 255)
    {
        block.push_back(255);
        extension -= 255;
    }
    block.push_back(extension);
    return block;
}

void ExpectInvalid(const std::vector<std::uint8_t>& data)
{
    startup_intro::PaniPackage package;
    assert(!startup_intro::ParsePani(data.data(), data.size(), package));
}
}

int main(int argc, char** argv)
{
    startup_intro::PaniPackage package;
    auto valid = MakePackage();
    assert(startup_intro::ParsePani(valid.data(), valid.size(), package));
    assert(package.frameCount == 1);
    assert(package.totalDuration == 30);
    assert(startup_intro::PaniFrameAtTick(package, 0) == 0);
    assert(startup_intro::PaniFrameAtTick(package, 29) == 0);
    assert(startup_intro::PaniFrameAtTick(package, 30) == package.frameCount);

    startup_intro::PaniPackage metadataPackage;
    assert(startup_intro::ParsePaniHeader(valid.data(), HeaderSize, valid.size(), metadataPackage));
    assert(startup_intro::ParsePaniFrameTable(valid.data() + HeaderSize + PaletteSize,
        FrameEntrySize, valid.size(), metadataPackage));
    assert(metadataPackage.frames[0].offset == HeaderSize + PaletteSize + FrameEntrySize);
    assert(!startup_intro::ParsePaniFrameTable(valid.data() + HeaderSize + PaletteSize,
        FrameEntrySize - 1, valid.size(), metadataPackage));

    std::vector<std::uint8_t> decoded(startup_intro::PaniFrameBytes);
    assert(startup_intro::DecodePaniFrame(0, valid.data() + package.frames[0].offset,
        package.frames[0].length, decoded.data(), decoded.size()));

    const auto lz4 = RepeatedByteLz4();
    assert(startup_intro::DecodePaniFrame(1, lz4.data(), lz4.size(), decoded.data(), decoded.size()));
    for (std::uint8_t value : decoded)
        assert(value == 7);

    assert(!startup_intro::DecodePaniFrame(1, nullptr, 0, decoded.data(), decoded.size()));
    const std::vector<std::uint8_t> truncatedLiteral{0x10};
    assert(!startup_intro::DecodePaniFrame(1, truncatedLiteral.data(), truncatedLiteral.size(),
        decoded.data(), decoded.size()));
    const std::vector<std::uint8_t> truncatedExtendedLength{0xf0, 255};
    assert(!startup_intro::DecodePaniFrame(1, truncatedExtendedLength.data(),
        truncatedExtendedLength.size(), decoded.data(), decoded.size()));
    const std::vector<std::uint8_t> zeroOffset{0x00, 0x00, 0x00};
    assert(!startup_intro::DecodePaniFrame(1, zeroOffset.data(), zeroOffset.size(),
        decoded.data(), decoded.size()));
    const std::vector<std::uint8_t> invalidBackReference{0x10, 1, 2, 0};
    assert(!startup_intro::DecodePaniFrame(1, invalidBackReference.data(), invalidBackReference.size(),
        decoded.data(), decoded.size()));
    const std::vector<std::uint8_t> outputUnderflow{0x10, 1};
    assert(!startup_intro::DecodePaniFrame(1, outputUnderflow.data(), outputUnderflow.size(),
        decoded.data(), decoded.size()));
    auto outputOverflow = lz4;
    outputOverflow.back()++;
    assert(!startup_intro::DecodePaniFrame(1, outputOverflow.data(), outputOverflow.size(),
        decoded.data(), decoded.size()));
    auto trailingSource = lz4;
    trailingSource.push_back(0);
    trailingSource.push_back(1);
    assert(!startup_intro::DecodePaniFrame(1, trailingSource.data(), trailingSource.size(),
        decoded.data(), decoded.size()));

    auto twoFrames = MakePackage(2, 30);
    assert(startup_intro::ParsePani(twoFrames.data(), twoFrames.size(), package));
    assert(startup_intro::PaniFrameAtTick(package, 29) == 0);
    assert(startup_intro::PaniFrameAtTick(package, 30) == 1);
    assert(startup_intro::PaniFrameAtTick(package, 59) == 1);

    auto badMagic = valid;
    badMagic[0] = 'X';
    ExpectInvalid(badMagic);

    auto badVersion = valid;
    WriteLe16(badVersion, 4, 2);
    ExpectInvalid(badVersion);

    auto truncatedTable = valid;
    truncatedTable.resize(HeaderSize + PaletteSize + FrameEntrySize - 1);
    WriteLe32(truncatedTable, 24, truncatedTable.size());
    ExpectInvalid(truncatedTable);

    auto truncatedFrame = valid;
    truncatedFrame.resize(truncatedFrame.size() - 1);
    WriteLe32(truncatedFrame, 24, truncatedFrame.size());
    ExpectInvalid(truncatedFrame);

    auto overflowingOffset = valid;
    WriteLe32(overflowingOffset, HeaderSize + PaletteSize, 0xFFFFFFF0u);
    ExpectInvalid(overflowingOffset);

    auto overlappingFrame = valid;
    WriteLe32(overlappingFrame, HeaderSize + PaletteSize, HeaderSize);
    ExpectInvalid(overlappingFrame);

    auto wrongFrameSize = valid;
    WriteLe32(wrongFrameSize, HeaderSize + PaletteSize + 4, startup_intro::PaniFrameBytes - 1);
    ExpectInvalid(wrongFrameSize);

    auto tooShort = MakePackage(1, 29);
    ExpectInvalid(tooShort);

    auto tooLong = MakePackage(1, 601);
    ExpectInvalid(tooLong);

    auto tooManyFrames = MakePackage(startup_intro::PaniMaxFrames + 1, 1);
    ExpectInvalid(tooManyFrames);

    std::vector<std::uint8_t> tooLarge(startup_intro::PaniMaxPackageBytes + 1);
    ExpectInvalid(tooLarge);

    auto compressed = MakePackage(1, 30, 1, lz4.size());
    const std::size_t compressedEntry = HeaderSize + PaletteSize;
    const std::uint32_t compressedOffset = RoundUpSector(compressedEntry + FrameEntrySize);
    std::copy(lz4.begin(), lz4.end(), compressed.begin() + compressedOffset);
    assert(startup_intro::ParsePani(compressed.data(), compressed.size(), package));
    assert(package.codec == 1);
    assert(package.frames[0].offset % SectorSize == 0);
    assert(package.frames[0].length == lz4.size());

    auto unalignedCompressedOffset = compressed;
    WriteLe32(unalignedCompressedOffset, compressedEntry, compressedOffset + 1);
    ExpectInvalid(unalignedCompressedOffset);

    auto truncatedCompressedPadding = compressed;
    truncatedCompressedPadding.resize(compressedOffset + lz4.size());
    WriteLe32(truncatedCompressedPadding, 24, truncatedCompressedPadding.size());
    ExpectInvalid(truncatedCompressedPadding);

    auto overlappingCompressedPadding = MakePackage(2, 15, 1, lz4.size());
    const std::size_t secondCompressedEntry = compressedEntry + FrameEntrySize;
    WriteLe32(overlappingCompressedPadding, secondCompressedEntry, compressedOffset + lz4.size());
    ExpectInvalid(overlappingCompressedPadding);

    auto zeroCompressedSize = compressed;
    WriteLe32(zeroCompressedSize, HeaderSize + PaletteSize + 4, 0);
    ExpectInvalid(zeroCompressedSize);

    auto tooLargeCompressedSize = compressed;
    WriteLe32(tooLargeCompressedSize, HeaderSize + PaletteSize + 4,
        startup_intro::PaniFrameBytes + 1);
    ExpectInvalid(tooLargeCompressedSize);

    auto unknownCodec = valid;
    WriteLe16(unknownCodec, 14, 2);
    ExpectInvalid(unknownCodec);

    auto unknownField = valid;
    WriteLe32(unknownField, 28, 1);
    ExpectInvalid(unknownField);

    if (argc == 2)
    {
        std::ifstream fixture(argv[1], std::ios::binary);
        std::vector<std::uint8_t> fixtureData{
            std::istreambuf_iterator<char>(fixture), std::istreambuf_iterator<char>()};
        assert(startup_intro::ParsePani(fixtureData.data(), fixtureData.size(), package));
        for (std::uint16_t i = 0; i < package.frameCount; i++)
        {
            assert(startup_intro::DecodePaniFrame(package.codec,
                fixtureData.data() + package.frames[i].offset, package.frames[i].length,
                decoded.data(), decoded.size()));
        }
    }
}
