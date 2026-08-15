#include "common.h"
#include <algorithm>

#include "gui/font/nitroFont2.h"
#include "Nft2CoverageRenderer.h"

uint32_t Nft2CoverageRenderer::Render(const nft2_header_t* font, const char16_t* text, std::span<uint8_t> coverage,
    uint32_t width, uint32_t height, int x, int y, bool ellipsis)
{
    if (!font || !text || width == 0 || height == 0 || coverage.size() < width * height)
        return 0;

    std::fill(coverage.begin(), coverage.end(), 0);
    nft2_string_render_params_t parameters{ x, y, width, height, 0, true, false };
    if (ellipsis)
        nft2_renderStringEllipsis(font, text, coverage.data(), width, &parameters, u" ... ");
    else
        nft2_renderString(font, text, coverage.data(), width, &parameters);
    for (uint32_t index = 0; index < width * height; ++index)
        coverage[index] >>= 4;
    return parameters.textWidth;
}
