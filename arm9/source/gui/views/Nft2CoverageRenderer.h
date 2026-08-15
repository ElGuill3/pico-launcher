#pragma once

#include <cstdint>
#include <span>

struct nft2_header_t;

class Nft2CoverageRenderer
{
public:
    static constexpr bool PreservesFullCoverage = true;

    static uint32_t Render(const nft2_header_t* font, const char16_t* text, std::span<uint8_t> coverage,
        uint32_t width, uint32_t height, int x, int y, bool ellipsis);
};
