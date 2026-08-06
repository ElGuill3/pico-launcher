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

struct PaniFrame
{
    std::uint32_t offset;
    std::uint16_t duration;
};

struct PaniPackage
{
    std::uint32_t paletteOffset;
    std::uint16_t frameCount;
    std::uint16_t totalDuration;
    PaniFrame frames[PaniMaxFrames];
};

bool ParsePani(const std::uint8_t* data, std::size_t size, PaniPackage& package);
std::uint16_t PaniFrameAtTick(const PaniPackage& package, std::uint16_t tick);
}
