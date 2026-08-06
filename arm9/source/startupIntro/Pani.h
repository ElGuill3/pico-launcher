#pragma once

#include <cstddef>
#include <cstdint>

namespace startup_intro
{
constexpr std::uint16_t PaniWidth = 256;
constexpr std::uint16_t PaniHeight = 192;
constexpr std::uint32_t PaniFrameBytes = PaniWidth * PaniHeight;
constexpr std::uint16_t PaniMaxFrames = 150;
constexpr std::size_t PaniMaxPackageBytes = 4 * 1024 * 1024;
constexpr std::uint32_t PaniLz4BlockBytes = 512;

constexpr std::uint32_t PaniLz4StoredBytes(std::uint32_t length)
{
    return (length + PaniLz4BlockBytes - 1) & ~(PaniLz4BlockBytes - 1);
}

struct PaniFrame
{
    std::uint32_t offset;
    std::uint32_t length;
    std::uint16_t duration;
};

struct PaniPackage
{
    std::uint32_t paletteOffset;
    std::uint32_t tableOffset;
    std::uint16_t frameCount;
    std::uint16_t codec;
    std::uint16_t totalDuration;
    PaniFrame frames[PaniMaxFrames];
};

bool ParsePani(const std::uint8_t* data, std::size_t size, PaniPackage& package);
bool ParsePaniHeader(const std::uint8_t* data, std::size_t headerSize, std::size_t fileSize,
    PaniPackage& package);
bool ParsePaniFrameTable(const std::uint8_t* data, std::size_t tableSize, std::size_t fileSize,
    PaniPackage& package);
bool DecodePaniFrame(std::uint16_t codec, const std::uint8_t* source, std::size_t sourceSize,
    std::uint8_t* destination, std::size_t destinationSize);
std::uint16_t PaniFrameAtTick(const PaniPackage& package, std::uint16_t tick);
}
