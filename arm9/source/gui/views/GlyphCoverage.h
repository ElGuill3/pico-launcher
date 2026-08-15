#pragma once
#include <cstdint>

namespace glyph_coverage
{
constexpr uint8_t BinaryThreshold = 8;

constexpr uint8_t QuantizeByte(uint8_t pixels)
{
    const uint8_t low = (pixels & 0x0f) >= BinaryThreshold ? 0x0f : 0;
    const uint8_t high = (pixels >> 4) >= BinaryThreshold ? 0xf0 : 0;
    return low | high;
}
}
