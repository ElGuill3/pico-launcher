#include "Pani.h"

namespace startup_intro
{
namespace
{
constexpr std::uint16_t Version = 1;
constexpr std::uint16_t HeaderSize = 32;
constexpr std::uint16_t Raw8Codec = 0;
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

bool ParsePani(const std::uint8_t* data, std::size_t size, PaniPackage& package)
{
    if (!data || size < HeaderSize || size > PaniMaxPackageBytes)
        return false;
    if (data[0] != 'P' || data[1] != 'A' || data[2] != 'N' || data[3] != 'I')
        return false;
    if (ReadLe16(data + 4) != Version || ReadLe16(data + 6) != HeaderSize ||
        ReadLe16(data + 8) != PaniWidth || ReadLe16(data + 10) != PaniHeight)
        return false;

    const std::uint16_t frameCount = ReadLe16(data + 12);
    if (frameCount == 0 || frameCount > PaniMaxFrames || ReadLe16(data + 14) != Raw8Codec)
        return false;

    const std::uint32_t paletteOffset = ReadLe32(data + 16);
    const std::uint32_t tableOffset = ReadLe32(data + 20);
    const std::uint32_t declaredSize = ReadLe32(data + 24);
    if (declaredSize != size || ReadLe32(data + 28) != 0)
        return false;

    const std::uint32_t tableBytes = frameCount * FrameEntryBytes;
    if (!RangeFits(paletteOffset, PaletteBytes, size) || !RangeFits(tableOffset, tableBytes, size) ||
        RangesOverlap(0, HeaderSize, paletteOffset, PaletteBytes) ||
        RangesOverlap(0, HeaderSize, tableOffset, tableBytes) ||
        RangesOverlap(paletteOffset, PaletteBytes, tableOffset, tableBytes))
        return false;

    PaniPackage parsed = {};
    parsed.paletteOffset = paletteOffset;
    parsed.frameCount = frameCount;
    std::uint32_t totalDuration = 0;
    for (std::uint16_t i = 0; i < frameCount; i++)
    {
        const std::uint8_t* entry = data + tableOffset + i * FrameEntryBytes;
        const std::uint32_t frameOffset = ReadLe32(entry);
        const std::uint32_t frameLength = ReadLe32(entry + 4);
        const std::uint16_t duration = ReadLe16(entry + 8);
        if (frameLength != PaniFrameBytes || duration == 0 || ReadLe16(entry + 10) != 0 ||
            !RangeFits(frameOffset, frameLength, size) ||
            RangesOverlap(0, HeaderSize, frameOffset, frameLength) ||
            RangesOverlap(paletteOffset, PaletteBytes, frameOffset, frameLength) ||
            RangesOverlap(tableOffset, tableBytes, frameOffset, frameLength))
            return false;

        for (std::uint16_t previous = 0; previous < i; previous++)
        {
            if (RangesOverlap(parsed.frames[previous].offset, PaniFrameBytes, frameOffset, frameLength))
                return false;
        }

        totalDuration += duration;
        if (totalDuration > MaxDuration)
            return false;
        parsed.frames[i] = { frameOffset, duration };
    }

    if (totalDuration < MinDuration)
        return false;
    parsed.totalDuration = totalDuration;
    package = parsed;
    return true;
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
