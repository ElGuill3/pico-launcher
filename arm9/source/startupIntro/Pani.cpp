#include "Pani.h"

#include <cstring>

namespace startup_intro
{
namespace
{
constexpr std::uint16_t Version = 1;
constexpr std::uint16_t HeaderSize = 32;
constexpr std::uint16_t Raw8Codec = 0;
constexpr std::uint16_t Lz4Codec = 1;
constexpr std::uint32_t PaletteBytes = 256 * 2;
constexpr std::uint32_t FrameEntryBytes = 12;
constexpr std::uint16_t MinDuration = 30;
constexpr std::uint16_t MaxDuration = 600;

std::uint16_t ReadLe16(const std::uint8_t* data)
{
    return static_cast<std::uint16_t>(data[0]) |
        (static_cast<std::uint16_t>(data[1]) << 8);
}

std::uint32_t ReadLe32(const std::uint8_t* data)
{
    return static_cast<std::uint32_t>(data[0]) |
        (static_cast<std::uint32_t>(data[1]) << 8) |
        (static_cast<std::uint32_t>(data[2]) << 16) |
        (static_cast<std::uint32_t>(data[3]) << 24);
}

bool RangeFits(std::uint32_t offset, std::uint32_t length, std::size_t size)
{
    return offset <= size && length <= size - offset;
}

bool RangesOverlap(std::uint32_t firstOffset, std::uint32_t firstLength,
    std::uint32_t secondOffset, std::uint32_t secondLength)
{
    return firstOffset < secondOffset + secondLength &&
        secondOffset < firstOffset + firstLength;
}
}

bool ParsePaniHeader(const std::uint8_t* data, std::size_t headerSize, std::size_t fileSize,
    PaniPackage& package)
{
    if (!data || headerSize < HeaderSize || fileSize < HeaderSize || fileSize > PaniMaxPackageBytes)
        return false;
    if (data[0] != 'P' || data[1] != 'A' || data[2] != 'N' || data[3] != 'I')
        return false;
    if (ReadLe16(data + 4) != Version || ReadLe16(data + 6) != HeaderSize ||
        ReadLe16(data + 8) != PaniWidth || ReadLe16(data + 10) != PaniHeight)
        return false;

    const std::uint16_t frameCount = ReadLe16(data + 12);
    const std::uint16_t codec = ReadLe16(data + 14);
    if (frameCount == 0 || frameCount > PaniMaxFrames || (codec != Raw8Codec && codec != Lz4Codec))
        return false;

    const std::uint32_t paletteOffset = ReadLe32(data + 16);
    const std::uint32_t tableOffset = ReadLe32(data + 20);
    const std::uint32_t declaredSize = ReadLe32(data + 24);
    if (declaredSize != fileSize || ReadLe32(data + 28) != 0)
        return false;

    const std::uint32_t tableBytes = frameCount * FrameEntryBytes;
    if (!RangeFits(paletteOffset, PaletteBytes, fileSize) ||
        !RangeFits(tableOffset, tableBytes, fileSize) ||
        RangesOverlap(0, HeaderSize, paletteOffset, PaletteBytes) ||
        RangesOverlap(0, HeaderSize, tableOffset, tableBytes) ||
        RangesOverlap(paletteOffset, PaletteBytes, tableOffset, tableBytes))
        return false;

    package = {};
    package.paletteOffset = paletteOffset;
    package.tableOffset = tableOffset;
    package.frameCount = frameCount;
    package.codec = codec;
    return true;
}

bool ParsePaniFrameTable(const std::uint8_t* data, std::size_t tableSize, std::size_t fileSize,
    PaniPackage& package)
{
    const std::uint32_t tableBytes = package.frameCount * FrameEntryBytes;
    if (!data || tableSize != tableBytes || package.frameCount == 0 ||
        package.frameCount > PaniMaxFrames || fileSize > PaniMaxPackageBytes)
        return false;

    PaniPackage parsed = package;
    std::uint32_t totalDuration = 0;
    for (std::uint16_t i = 0; i < package.frameCount; i++)
    {
        const std::uint8_t* entry = data + i * FrameEntryBytes;
        const std::uint32_t frameOffset = ReadLe32(entry);
        const std::uint32_t frameLength = ReadLe32(entry + 4);
        const std::uint16_t duration = ReadLe16(entry + 8);
        const bool validLength = package.codec == Raw8Codec ? frameLength == PaniFrameBytes :
            frameLength > 0 && frameLength < PaniFrameBytes;
        const std::uint32_t storedLength = package.codec == Lz4Codec ?
            PaniLz4StoredBytes(frameLength) : frameLength;
        if (!validLength || (package.codec == Lz4Codec && frameOffset % PaniLz4BlockBytes != 0) ||
            duration == 0 || ReadLe16(entry + 10) != 0 ||
            !RangeFits(frameOffset, storedLength, fileSize) ||
            RangesOverlap(0, HeaderSize, frameOffset, storedLength) ||
            RangesOverlap(package.paletteOffset, PaletteBytes, frameOffset, storedLength) ||
            RangesOverlap(package.tableOffset, tableBytes, frameOffset, storedLength))
            return false;

        for (std::uint16_t previous = 0; previous < i; previous++)
        {
            const std::uint32_t previousStoredLength = package.codec == Lz4Codec ?
                PaniLz4StoredBytes(parsed.frames[previous].length) : parsed.frames[previous].length;
            if (RangesOverlap(parsed.frames[previous].offset, previousStoredLength,
                frameOffset, storedLength))
                return false;
        }

        totalDuration += duration;
        if (totalDuration > MaxDuration)
            return false;
        parsed.frames[i] = { frameOffset, frameLength, duration };
    }

    if (totalDuration < MinDuration)
        return false;
    parsed.totalDuration = totalDuration;
    package = parsed;
    return true;
}

bool ParsePani(const std::uint8_t* data, std::size_t size, PaniPackage& package)
{
    PaniPackage parsed;
    if (!ParsePaniHeader(data, size, size, parsed))
        return false;
    if (!ParsePaniFrameTable(data + parsed.tableOffset,
        parsed.frameCount * FrameEntryBytes, size, parsed))
        return false;
    package = parsed;
    return true;
}

bool DecodePaniFrame(std::uint16_t codec, const std::uint8_t* source, std::size_t sourceSize,
    std::uint8_t* destination, std::size_t destinationSize)
{
    if (!source || !destination || destinationSize != PaniFrameBytes)
        return false;
    if (codec == Raw8Codec)
    {
        if (sourceSize != destinationSize)
            return false;
        std::memcpy(destination, source, destinationSize);
        return true;
    }
    if (codec != Lz4Codec || sourceSize == 0 || sourceSize >= PaniFrameBytes)
        return false;

    const std::uint8_t* input = source;
    const std::uint8_t* const inputEnd = source + sourceSize;
    std::uint8_t* output = destination;
    std::uint8_t* const outputEnd = destination + destinationSize;
    while (input < inputEnd)
    {
        const std::uint8_t token = *input++;
        std::size_t literalLength = token >> 4;
        if (literalLength == 15)
        {
            std::uint8_t extension;
            do
            {
                if (input == inputEnd)
                    return false;
                extension = *input++;
                if (literalLength > destinationSize - extension)
                    return false;
                literalLength += extension;
            } while (extension == 255);
        }
        if (literalLength > static_cast<std::size_t>(inputEnd - input) ||
            literalLength > static_cast<std::size_t>(outputEnd - output))
            return false;
        std::memcpy(output, input, literalLength);
        input += literalLength;
        output += literalLength;
        if (input == inputEnd)
            return output == outputEnd;
        if (inputEnd - input < 2 || output == outputEnd)
            return false;

        const std::size_t offset = ReadLe16(input);
        input += 2;
        if (offset == 0 || offset > static_cast<std::size_t>(output - destination))
            return false;

        std::size_t matchLength = (token & 0x0f) + 4;
        if ((token & 0x0f) == 15)
        {
            std::uint8_t extension;
            do
            {
                if (input == inputEnd)
                    return false;
                extension = *input++;
                if (matchLength > destinationSize - extension)
                    return false;
                matchLength += extension;
            } while (extension == 255);
        }
        if (matchLength > static_cast<std::size_t>(outputEnd - output))
            return false;
        for (std::size_t i = 0; i < matchLength; i++)
            output[i] = *(output + i - offset);
        output += matchLength;
    }
    return output == outputEnd;
}

std::uint16_t PaniFrameAtTick(const PaniPackage& package, std::uint16_t tick)
{
    std::uint16_t endTick = 0;
    for (std::uint16_t i = 0; i < package.frameCount; i++)
    {
        endTick += package.frames[i].duration;
        if (tick < endTick)
            return i;
    }
    return package.frameCount;
}
}
