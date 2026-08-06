#pragma once
#include <cstdint>

namespace startup_intro
{
constexpr std::uint16_t HiddenBrightness = 0x8010;
constexpr std::uint16_t MainBackdropColor = 0;
constexpr std::uint32_t MainBackdropDisplayMode = 1u << 16;

constexpr std::uint16_t BlackBrightness(std::uint16_t level)
{
    return static_cast<std::uint16_t>(0x8000u | level);
}
}
