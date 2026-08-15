#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace status_background
{
constexpr size_t CustomTopBackgroundBytes = 256 * 192 * 2;
constexpr size_t StripBytes = 256 * 16 * 2;

struct StatusStripUploadPlan
{
    int left;
    int top;
    int right;
    int bottom;

    constexpr bool IsValid() const { return left < right && top < bottom; }
    constexpr size_t RowBytes() const { return IsValid() ? (right - left) * sizeof(uint16_t) : 0; }
    constexpr size_t Bytes() const { return RowBytes() * (bottom - top); }
};

constexpr StatusStripUploadPlan MakeStatusStripUploadPlan(int left, int top, int right, int bottom)
{
    return { std::clamp(left, 0, 256), std::clamp(top, 0, 16),
        std::clamp(right, 0, 256), std::clamp(bottom, 0, 16) };
}

constexpr StatusStripUploadPlan MergeStatusStripUploadPlans(
    StatusStripUploadPlan first, StatusStripUploadPlan second)
{
    if (!first.IsValid())
        return second;
    if (!second.IsValid())
        return first;
    return { std::min(first.left, second.left), std::min(first.top, second.top),
        std::max(first.right, second.right), std::max(first.bottom, second.bottom) };
}

enum class StatusPresentationMode
{
    ComposedBackground,
    UniformObject,
    Unsupported,
};

constexpr StatusPresentationMode CustomCompositionMode(bool loaded, bool stripsAvailable)
{
    return loaded && stripsAvailable ? StatusPresentationMode::ComposedBackground : StatusPresentationMode::Unsupported;
}

constexpr bool HasExactCustomTopBackgroundSize(size_t size)
{
    return size == CustomTopBackgroundBytes;
}

inline bool RebuildCustomStrip(std::span<const uint8_t> pristine, std::span<uint8_t> composed)
{
    if (pristine.size() != StripBytes || composed.size() != StripBytes)
        return false;
    std::copy(pristine.begin(), pristine.end(), composed.begin());
    return true;
}

inline bool HasUniformMaterialTopStrip(std::span<const uint16_t> map, std::span<const uint8_t> tiles)
{
    if (map.size() < 64 || tiles.empty())
        return false;
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 256; ++x)
        {
            const uint16_t mapEntry = map[(y / 8) * 32 + x / 8];
            const size_t tileOffset = (mapEntry & 0x3ff) * 64;
            if (tileOffset + 64 > tiles.size() ||
                tiles[tileOffset + (y % 8) * 8 + x % 8] != 16)
                return false;
        }
    }
    return true;
}

template<typename Word, size_t Size>
std::span<const uint8_t> MaterialTileBytes(const Word (&tiles)[Size])
{
    static_assert(sizeof(Word) == sizeof(uint32_t));
    return { reinterpret_cast<const uint8_t*>(tiles), sizeof(tiles) };
}

template<typename Word, size_t Size>
std::span<const uint8_t> MaterialTileBytes(const std::array<Word, Size>& tiles)
{
    static_assert(sizeof(Word) == sizeof(uint32_t));
    return { reinterpret_cast<const uint8_t*>(tiles.data()), sizeof(tiles) };
}

constexpr StatusPresentationMode MaterialCompositionMode(bool loaded, bool uniform)
{
    return loaded && uniform ? StatusPresentationMode::UniformObject : StatusPresentationMode::Unsupported;
}

constexpr uint16_t MaterialPaletteEndpoint(uint16_t paletteEntry16)
{
    return paletteEntry16;
}
}
