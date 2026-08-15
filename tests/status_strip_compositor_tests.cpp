#include <array>
#include <cassert>
#include <cstdint>
#include <span>

#include "themes/background/StatusStripComposition.h"
#include "themes/background/StatusBackgroundContracts.h"

namespace
{
constexpr uint16_t OpaqueBlack = 0x8000;
constexpr uint16_t OpaqueRed = 0x801f;

void AssertCanaries(const std::array<uint16_t, 4>& canaries)
{
    for (uint16_t value : canaries)
        assert(value == 0x55aa);
}
}

int main()
{
    using namespace status_strip;

    std::array<uint16_t, PixelCount> pristine;
    pristine.fill(OpaqueBlack);
    std::array<uint16_t, PixelCount> destination = pristine;
    std::array<uint8_t, 4> coverage{ 0, 1, 8, 15 };

    auto result = StatusStripCompositor::Compose({ pristine, destination, coverage, 0, 0, 4, 1, OpaqueRed });
    assert(result.state == CompositionState::Composed);
    assert(result.dirty);
    assert(destination[0] == OpaqueBlack);
    assert(destination[1] == 0x8002);
    assert(destination[2] == 0x8011);
    assert(destination[3] == OpaqueRed);

    destination[0] = OpaqueRed;
    result = StatusStripCompositor::Compose({ pristine, destination, {}, 0, 0, 0, 0, OpaqueRed });
    assert(result.state == CompositionState::Pristine);
    assert(result.dirty);
    assert(destination == pristine);

    std::array<uint16_t, PixelCount + 8> guardedDestination;
    guardedDestination.fill(0x55aa);
    std::span<uint16_t> guardedStrip(guardedDestination.data() + 4, PixelCount);
    std::copy(pristine.begin(), pristine.end(), guardedStrip.begin());
    std::array<uint8_t, 4> clippedCoverage{ 15, 15, 15, 15 };
    result = StatusStripCompositor::Compose({ pristine, guardedStrip, clippedCoverage, -1, 15, 2, 2, OpaqueRed });
    assert(result.state == CompositionState::Composed);
    assert(guardedStrip[15 * ScreenWidth] == OpaqueRed);
    assert(guardedStrip[15 * ScreenWidth + 1] == OpaqueBlack);
    AssertCanaries({ guardedDestination[0], guardedDestination[1], guardedDestination[2], guardedDestination[3] });
    AssertCanaries({ guardedDestination[PixelCount + 4], guardedDestination[PixelCount + 5],
        guardedDestination[PixelCount + 6], guardedDestination[PixelCount + 7] });

    std::array<uint16_t, PixelCount - 1> shortPristine{};
    const auto beforeInvalid = destination;
    result = StatusStripCompositor::Compose({ shortPristine, destination, coverage, 0, 0, 4, 1, OpaqueRed });
    assert(result.state == CompositionState::Invalid);
    assert(!result.dirty);
    assert(destination == beforeInvalid);

    using namespace status_background;
    static_assert(CustomTopBackgroundBytes == 256 * 192 * 2);
    assert(HasExactCustomTopBackgroundSize(CustomTopBackgroundBytes));
    assert(!HasExactCustomTopBackgroundSize(CustomTopBackgroundBytes - 1));
    assert(!HasExactCustomTopBackgroundSize(CustomTopBackgroundBytes + 1));
    assert(CustomCompositionMode(true, true) == StatusPresentationMode::ComposedBackground);
    assert(CustomCompositionMode(false, true) == StatusPresentationMode::Unsupported);
    assert(CustomCompositionMode(true, false) == StatusPresentationMode::Unsupported);

    std::array<uint8_t, StripBytes> sourceStrip;
    sourceStrip.fill(0x12);
    std::array<uint8_t, StripBytes> pristineStrip = sourceStrip;
    std::array<uint8_t, StripBytes> composedStrip;
    composedStrip.fill(0x34);
    assert(RebuildCustomStrip(pristineStrip, composedStrip));
    assert(composedStrip == pristineStrip);
    assert(!RebuildCustomStrip(std::span<const uint8_t>(sourceStrip.data(), StripBytes - 1), composedStrip));

    constexpr auto upload = MakeStatusStripUploadPlan(73, 0, 236, 16);
    static_assert(upload.left == 73 && upload.right == 236);
    static_assert(upload.top == 0 && upload.bottom == 16);
    static_assert(upload.RowBytes() == (236 - 73) * 2);
    static_assert(upload.Bytes() == (236 - 73) * 16 * 2);
    static_assert(!MakeStatusStripUploadPlan(256, 0, 256, 16).IsValid());
    static_assert(MakeStatusStripUploadPlan(-5, -5, 3, 2).left == 0);

    constexpr auto singleRow = MakeStatusStripUploadPlan(190, 0, 256, 1);
    static_assert(singleRow.RowBytes() == 132);
    static_assert(singleRow.Bytes() == 132);
    constexpr auto visibleRows = MakeStatusStripUploadPlan(73, 0, 236, 16);
    static_assert(visibleRows.Bytes() < StripBytes);

    std::array<uint8_t, 64> uniformTile;
    uniformTile.fill(16);
    std::array<uint16_t, 64> uniformMap{};
    assert(HasUniformMaterialTopStrip(uniformMap, uniformTile));
    uniformTile[63] = 15;
    assert(!HasUniformMaterialTopStrip(uniformMap, uniformTile));
    assert(MaterialCompositionMode(true, true) == StatusPresentationMode::UniformObject);
    assert(MaterialCompositionMode(true, false) == StatusPresentationMode::Unsupported);
    assert(MaterialCompositionMode(false, true) == StatusPresentationMode::Unsupported);
    assert(MaterialCompositionMode(false, false) == StatusPresentationMode::Unsupported);
    assert(MaterialPaletteEndpoint(0x1234) == 0x1234);

    std::array<uint32_t, 16> packedUniformTiles;
    packedUniformTiles.fill(0x10101010);
    assert(HasUniformMaterialTopStrip(uniformMap, MaterialTileBytes(packedUniformTiles)));
    packedUniformTiles[15] = 0x0f101010;
    assert(!HasUniformMaterialTopStrip(uniformMap, MaterialTileBytes(packedUniformTiles)));
}
