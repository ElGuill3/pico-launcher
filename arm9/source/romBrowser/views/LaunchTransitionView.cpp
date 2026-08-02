#include "common.h"
#include <algorithm>
#include <libtwl/dma/dmaNitro.h>
#include <nds/arm9/cache.h>
#include "gui/Gx.h"
#include "gui/GraphicsContext.h"
#include "gui/OamBuilder.h"
#include "gui/VramContext.h"
#include "gui/IVramManager.h"
#include "gui/materialDesign.h"
#include "gui/palette/DirectPalette.h"
#include "themes/IFontRepository.h"
#include "themes/FontType.h"
#include "../FileType/Nds/NdsFileIcon.h"
#include "../Theme/IThemeFileIconFactory.h"
#include "LaunchTransitionTimeline.h"
#include "LaunchTransitionView.h"

namespace
{
void DrawSolidQuad(GraphicsContext& graphicsContext, const Rgb<8, 8, 8>& color,
    int alpha, int polygonId, int left, int top, int right, int bottom)
{
    if (left >= right || top >= bottom)
        return;

    Gx::MtxIdentity();
    Gx::PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGON_MODE_MODULATE, GX_DISPLAY_MODE_FRONT,
        false, false, false, GX_DEPTH_FUNC_LESS, false, alpha, polygonId);
    Gx::TexImageParam((128 * 1024) >> 3, false, false, false, false, GX_TEXSIZE_8,
        GX_TEXSIZE_8, GX_TEXFMT_PLTT16, false, GX_TEXGEN_NONE);
    graphicsContext.GetRgb6Palette()->ApplyColor(Rgb<6, 6, 6>(color));
    const int z = -(polygonId - 60) * 64;
    Gx::Begin(GX_PRIMITIVE_QUAD);
    Gx::TexCoord(0, 0);
    REG_GX_VTX_16 = GX_VTX_PACK(left << 6, top << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(left << 6, bottom << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(right << 6, bottom << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(right << 6, top << 3);
    REG_GX_VTX_16 = z;
    Gx::End();
}

void DrawSolidTrapezoid(GraphicsContext& graphicsContext, const Rgb<8, 8, 8>& color,
    int polygonId, int topLeft, int topRight, int bottomLeft, int bottomRight,
    int top, int bottom)
{
    if (topLeft >= topRight || bottomLeft >= bottomRight)
        return;

    Gx::MtxIdentity();
    Gx::PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGON_MODE_MODULATE, GX_DISPLAY_MODE_FRONT,
        false, false, false, GX_DEPTH_FUNC_LESS, false, 31, polygonId);
    Gx::TexImageParam((128 * 1024) >> 3, false, false, false, false, GX_TEXSIZE_8,
        GX_TEXSIZE_8, GX_TEXFMT_PLTT16, false, GX_TEXGEN_NONE);
    graphicsContext.GetRgb6Palette()->ApplyColor(Rgb<6, 6, 6>(color));
    const int z = -(polygonId - 60) * 64;
    Gx::Begin(GX_PRIMITIVE_QUAD);
    Gx::TexCoord(0, 0);
    REG_GX_VTX_16 = GX_VTX_PACK(topLeft << 6, top << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(bottomLeft << 6, bottom << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(bottomRight << 6, bottom << 3);
    REG_GX_VTX_16 = z;
    REG_GX_VTX_16 = GX_VTX_PACK(topRight << 6, top << 3);
    REG_GX_VTX_16 = z;
    Gx::End();
}
}

LaunchTransitionView::LaunchTransitionView(LaunchVisualSnapshot snapshot,
    const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository,
    const IThemeFileIconFactory* themeFileIconFactory,
    VBlankTextureLoader* vblankTextureLoader)
    : _snapshot(std::move(snapshot))
    , _materialColorScheme(materialColorScheme)
    , _themeFileIconFactory(themeFileIconFactory)
    , _vblankTextureLoader(vblankTextureLoader)
    , _titleLabel(Label2DView::CreateShared(192, 16, LaunchVisualSnapshot::TitleLength - 1,
        fontRepository->GetFont(FontType::Medium11)))
    , _transformAnimator(0, LaunchTransitionTimeline::ProgressMax,
        LaunchTransitionTimeline::TransformFrames, &md::sys::motion::easing::emphasized)
{
    _titleLabel->SetPosition(32, 88);
    _titleLabel->SetHorizontalAlignment(Alignment::Center);
    _titleLabel->SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    _titleLabel->SetBackgroundColor(_materialColorScheme->scrim);
    _titleLabel->SetForegroundColor(_materialColorScheme->onSurface);
    _titleLabel->SetText(_snapshot.title);
}

LaunchTransitionView::~LaunchTransitionView()
{
    _vblankTextureLoader->CancelLoad(_coverLoadRequest);
}

void LaunchTransitionView::InitVram(const VramContext& vramContext)
{
    auto texVramManager = vramContext.GetTexVramManager();
    auto texPlttVramManager = vramContext.GetTexPlttVramManager();
    auto objVramManager = vramContext.GetObjVramManager();

    if (_snapshot.cover && texVramManager && texPlttVramManager)
    {
        _coverTexVramOffset = texVramManager->Alloc(128 * 96);
        _coverPlttVramOffset = texPlttVramManager->Alloc(256 * 2);
        _snapshot.cover->SetTexVramOffset(_coverTexVramOffset, _coverPlttVramOffset);
        _coverLoadRequest = _snapshot.cover->CreateTextureLoadRequest();
        _vblankTextureLoader->RequestLoad(_coverLoadRequest);
    }

    if (objVramManager)
    {
        _iconVramOffset = objVramManager->Alloc(FILE_ICON_VRAM_SIZE);
        _iconVramAddress = objVramManager->GetVramAddress(_iconVramOffset);
        if (_snapshot.ndsBanner)
        {
            DC_FlushRange(_snapshot.ndsBanner.get(), sizeof(*_snapshot.ndsBanner));
            _ndsIcon = std::make_unique<NdsFileIcon>(_snapshot.ndsBanner.get());
            _ndsIcon->SetVramAddress(_iconVramAddress, _iconVramOffset);
            _ndsIcon->UploadGraphics();
        }
        else if (_snapshot.hasIcon)
        {
            DC_FlushRange(_snapshot.iconGraphics, LaunchVisualSnapshot::IconGraphicsSize);
            dma_ntrCopy32(3, _snapshot.iconGraphics, _iconVramAddress,
                LaunchVisualSnapshot::IconGraphicsSize);
        }
        else
        {
            _fallbackIcon = _snapshot.isNds
                ? _themeFileIconFactory->CreateNdsFileIcon("")
                : _themeFileIconFactory->CreateGenericFileIcon("");
            if (_fallbackIcon)
            {
                _fallbackIcon->SetVramAddress(_iconVramAddress, _iconVramOffset);
                _fallbackIcon->UploadGraphics();
            }
        }
    }

    _titleLabel->InitVram(vramContext);
}

void LaunchTransitionView::Update()
{
    _transformAnimator.Update();
    _frame++;
    _titleLabel->Update();
    if (_ndsIcon && IsMinimumComplete())
        _ndsIcon->Update();
    if (_fallbackIcon)
        _fallbackIcon->Update();
}

void LaunchTransitionView::Draw(GraphicsContext& graphicsContext)
{
    const int progress = _transformAnimator.GetValue();
    DrawCover(graphicsContext, progress);
    DrawScrimAndProgress(graphicsContext, progress);

    if (IsMinimumComplete())
    {
        const u32 oldPriority = graphicsContext.SetPriority(0);
        DrawIcon(graphicsContext);
        _titleLabel->Draw(graphicsContext);
        graphicsContext.SetPriority(oldPriority);
    }
}

void LaunchTransitionView::VBlank()
{
    _titleLabel->VBlank();
}

void LaunchTransitionView::DrawCover(GraphicsContext& graphicsContext, int progress) const
{
    if (!_snapshot.cover || _coverLoadRequest.GetState() != VBlankTextureLoadRequestState::LoadComplete)
        return;

    const int inverse = LaunchTransitionTimeline::ProgressMax - progress;
    const int left = 75 * inverse / LaunchTransitionTimeline::ProgressMax;
    const int right = 256 - left;
    const int top = 48 * inverse / LaunchTransitionTimeline::ProgressMax;
    const int bottom = 192 - top;
    const int textureTop = 8 * progress / LaunchTransitionTimeline::ProgressMax;
    const int alpha = LaunchTransitionTimeline::CoverAlpha(progress);

    Gx::MtxIdentity();
    Gx::TexImageParam(_coverTexVramOffset >> 3, false, true, false, true,
        GX_TEXSIZE_128, GX_TEXSIZE_128, GX_TEXFMT_PLTT256, false, GX_TEXGEN_NONE);
    Gx::TexPlttBase(_coverPlttVramOffset >> 4);
    Gx::PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGON_MODE_MODULATE, GX_DISPLAY_MODE_FRONT,
        false, false, false, GX_DEPTH_FUNC_LESS, false, alpha, 60);
    Gx::Begin(GX_PRIMITIVE_QUAD);
    Gx::TexCoord(0.5, -COVER_HEIGHT + textureTop + 0.5);
    REG_GX_VTX_16 = GX_VTX_PACK(left << 6, top << 3);
    REG_GX_VTX_16 = 0;
    Gx::TexCoord(0.5, -textureTop + 0.5);
    REG_GX_VTX_16 = GX_VTX_PACK(left << 6, bottom << 3);
    REG_GX_VTX_16 = 0;
    Gx::TexCoord(COVER_WIDTH - 0.5, -textureTop + 0.5);
    REG_GX_VTX_16 = GX_VTX_PACK(right << 6, bottom << 3);
    REG_GX_VTX_16 = 0;
    Gx::TexCoord(COVER_WIDTH - 0.5, -COVER_HEIGHT + textureTop + 0.5);
    REG_GX_VTX_16 = GX_VTX_PACK(right << 6, top << 3);
    REG_GX_VTX_16 = 0;
    Gx::End();
}

void LaunchTransitionView::DrawScrimAndProgress(GraphicsContext& graphicsContext, int progress) const
{
    const int scrimAlpha = LaunchTransitionTimeline::ScrimAlpha(progress);
    DrawSolidQuad(graphicsContext, _materialColorScheme->scrim, scrimAlpha, 61, 0, 0, 256, 192);

    if (!IsMinimumComplete())
        return;

    constexpr int barLeft = 48;
    constexpr int barTop = 122;
    constexpr int barRight = 208;
    constexpr int barBottom = 128;
    DrawSolidQuad(graphicsContext, _materialColorScheme->surfaceContainerHighest,
        31, 62, barLeft, barTop, barRight, barBottom);

    const int phase = LaunchTransitionTimeline::ProgressBarPhase(_frame);
    for (int x = barLeft - 20 + phase; x < barRight; x += 20)
    {
        const int topLeft = std::max(x + 4, barLeft);
        const int topRight = std::min(x + 16, barRight);
        const int bottomLeft = std::max(x, barLeft);
        const int bottomRight = std::min(x + 12, barRight);
        DrawSolidTrapezoid(graphicsContext, _materialColorScheme->primary,
            63, topLeft, topRight, bottomLeft, bottomRight, barTop, barBottom);
    }
}

void LaunchTransitionView::DrawIcon(GraphicsContext& graphicsContext)
{
    constexpr int iconX = 112;
    constexpr int iconY = 48;
    if (_ndsIcon)
    {
        _ndsIcon->SetPosition(iconX, iconY);
        _ndsIcon->Draw(graphicsContext, _materialColorScheme->scrim);
    }
    else if (_snapshot.hasIcon && _iconVramAddress)
    {
        const u32 paletteRow = graphicsContext.GetPaletteManager().AllocRow(
            DirectPalette(_snapshot.iconPalette), iconY, iconY + 32);
        auto builder = OamBuilder::OamWithSize<32, 32>(iconX, iconY, _iconVramOffset >> 7)
            .WithPalette16(paletteRow)
            .WithPriority(graphicsContext.GetPriority());
        auto oam = graphicsContext.GetOamManager().AllocOams(1);
        builder.Build(oam[0]);
    }
    else if (_fallbackIcon)
    {
        _fallbackIcon->SetPosition(iconX, iconY);
        _fallbackIcon->Draw(graphicsContext, _materialColorScheme->scrim);
    }
}
