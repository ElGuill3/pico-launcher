#include <cassert>
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

std::vector<std::uint8_t> MakePackage(std::uint16_t frameCount = 1, std::uint16_t duration = 30)
{
    const std::size_t tableOffset = HeaderSize + PaletteSize;
    const std::size_t framesOffset = tableOffset + frameCount * FrameEntrySize;
    const std::size_t fileSize = framesOffset + frameCount * startup_intro::PaniFrameBytes;
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
    WriteLe16(data, 14, 0);
    WriteLe32(data, 16, HeaderSize);
    WriteLe32(data, 20, tableOffset);
    WriteLe32(data, 24, fileSize);

    for (std::uint16_t i = 0; i < frameCount; i++)
    {
        const std::size_t entry = tableOffset + i * FrameEntrySize;
        WriteLe32(data, entry, framesOffset + i * startup_intro::PaniFrameBytes);
        WriteLe32(data, entry + 4, startup_intro::PaniFrameBytes);
        WriteLe16(data, entry + 8, duration);
    }
    return data;
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

    auto unknownCodec = valid;
    WriteLe16(unknownCodec, 14, 1);
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
        assert(package.frameCount == 3);
        assert(package.totalDuration == 45);
    }
}
