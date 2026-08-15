#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <span>
#include <string>

#include <nds/ndstypes.h>
#include "gui/font/nitroFont2.h"
#include "gui/views/Nft2CoverageRenderer.h"
#include "romBrowser/views/StatusBarFormat.h"
#include "themes/background/StatusStripComposition.h"

namespace
{
std::string ReadFile(const char* path)
{
    std::ifstream stream(path);
    return { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
}

void AssertCanaries(const std::array<uint8_t, status_bar::StatusCoverageBytes + 4>& guarded)
{
    assert(guarded[0] == 0xa5);
    assert(guarded[1] == 0xa5);
    assert(guarded[guarded.size() - 2] == 0xa5);
    assert(guarded[guarded.size() - 1] == 0xa5);
}
}

void nft2_renderString(const nft2_header_t*, const char16_t*, u8* destination, u32 stride,
    nft2_string_render_params_t* parameters)
{
    std::fill(destination, destination + stride * parameters->height, 0xf0);
    parameters->textWidth = parameters->width;
}

void nft2_renderStringEllipsis(const nft2_header_t* font, const char16_t* text, u8* destination, u32 stride,
    nft2_string_render_params_t* parameters, const char16_t*)
{
    nft2_renderString(font, text, destination, stride, parameters);
}

int main()
{
    static_assert(status_bar::StatusCoverageBytes ==
        status_bar::NicknameTextWidth * status_strip::ScreenHeight);

    std::array<uint8_t, status_bar::StatusCoverageBytes + 4> guarded{};
    guarded.fill(0xa5);
    auto coverage = std::span<uint8_t>(guarded).subspan(2, status_bar::StatusCoverageBytes);
    nft2_header_t font{};

    for (uint32_t width : { 64u, 24u, static_cast<uint32_t>(status_bar::NicknameTextWidth) })
    {
        assert(Nft2CoverageRenderer::Render(&font, u"status", coverage, width,
            status_strip::ScreenHeight, 0, 0, width == status_bar::NicknameTextWidth) == width);
        assert(std::all_of(coverage.begin(), coverage.begin() + width * status_strip::ScreenHeight,
            [](uint8_t value) { return value == 0x0f; }));
        AssertCanaries(guarded);
    }

    const auto viewHeader = ReadFile("arm9/source/romBrowser/views/RomBrowserTopScreenView.h");
    assert(viewHeader.find("std::array<uint8_t, status_bar::StatusCoverageBytes> _statusCoverage") != std::string::npos);
    assert(viewHeader.find("64 * 16") == std::string::npos);
}
