#pragma once
#include "LabelView.h"

class Label2DView : public LabelView
{
    SHARED_ONLY(Label2DView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;
    void SetDirectPalette(const uint16_t* palette) { _directPalette = palette; }

private:
    Label2DView(u32 width, u32 height, u32 maxStringLength, const nft2_header_t* font)
        : LabelView(width, height, maxStringLength, font, false) { }

    void UpdateTileBuffer() override;

    bool _tileBufferUpdated = false;
    u32 _vramOffset = 0;
    vu16* _vramAddress = nullptr;
    const uint16_t* _directPalette = nullptr;
};
