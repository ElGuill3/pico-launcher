#include "common.h"
#include <array>
#include <string.h>
#include <libtwl/dma/dmaNitro.h>
#include <libtwl/gfx/gfxStatus.h>
#include <nds/arm9/cache.h>
#include <nds/arm9/background.h>
#include "../ITheme.h"
#include "CustomSubBackground.h"

void CustomSubBackground::VBlank()
{
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0xF) | 5 | (4 << 8);
    REG_BG2CNT_SUB = BG_BMP16_256x256 | BG_PRIORITY_3 | BG_COLOR_16 | BG_MAP_BASE(2);
    REG_BG2HOFS_SUB = 0;
    REG_BG2VOFS_SUB = 0;
    REG_BG2X_SUB = 0;
    REG_BG2Y_SUB = 0;
    REG_BG2PA_SUB = 256;
    REG_BG2PB_SUB = 0;
    REG_BG2PC_SUB = 0;
    REG_BG2PD_SUB = 256;

    if (_statusStripDirty && _statusStripUpload.IsValid())
    {
        constexpr int VBlankEndLine = 213;
        constexpr size_t BytesPerLine = 2048;
        const int requiredLines = (_statusStripUpload.Bytes() + BytesPerLine - 1) / BytesPerLine;
        if (gfx_getVCount() + requiredLines >= VBlankEndLine)
            return;
        for (int y = _statusStripUpload.top; y < _statusStripUpload.bottom; ++y)
        {
            const size_t offset = (y * status_strip::ScreenWidth + _statusStripUpload.left) * sizeof(uint16_t);
            DC_FlushRange(_composedStrip.data() + offset, _statusStripUpload.RowBytes());
            dma_ntrCopy32(3, _composedStrip.data() + offset, (u8*)BG_GFX_SUB + 0x8000 + offset,
                _statusStripUpload.RowBytes());
            dma_ntrWait(3);
        }
        _statusStripDirty = false;
        _publishedStatusStripUpload = _statusStripUpload;
        _statusStripUpload = { 0, 0, 0, 0 };
    }
}

void CustomSubBackground::LoadResources(const ITheme& theme, const VramContext& vramContext)
{
    _statusStripAvailable = false;
    _statusStripDirty = false;
    _statusStripUpload = { 0, 0, 0, 0 };
    _publishedStatusStripUpload = { 0, 0, 0, 0 };
    auto tmpBuf = std::unique_ptr<u8[]>(new(cache_align) u8[status_background::CustomTopBackgroundBytes]);
    const auto file = std::make_unique<File>();
    if (theme.OpenThemeFile(*file, "topbg.bin"))
    {
        u32 bytesRead = 0;
        file->Read(tmpBuf.get(), status_background::CustomTopBackgroundBytes, bytesRead);
        if (status_background::HasExactCustomTopBackgroundSize(bytesRead))
        {
            memcpy((u8*)BG_GFX_SUB + 0x8000, tmpBuf.get(), status_background::CustomTopBackgroundBytes);
            memcpy(_pristineStrip.data(), tmpBuf.get(), status_background::StripBytes);
            memcpy(_composedStrip.data(), _pristineStrip.data(), status_background::StripBytes);
            _statusStripAvailable = true;
        }
        file->Close();
    }
}

status_strip::StatusCompositionResult CustomSubBackground::PrepareStatusStrip(
    const status_strip::StatusStripRequest& request)
{
    if (!_statusStripAvailable)
        return { status_strip::CompositionState::Invalid, false };
    auto composed = request;
    composed.pristine = std::span<const uint16_t>((const uint16_t*)_pristineStrip.data(), status_strip::PixelCount);
    composed.destination = std::span<uint16_t>((uint16_t*)_composedStrip.data(), status_strip::PixelCount);
    const auto result = status_strip::StatusStripCompositor::Compose(composed);
    if (result.dirty)
    {
        _statusStripUpload = status_background::MergeStatusStripUploadPlans(_statusStripUpload,
            status_background::MakeStatusStripUploadPlan(request.left, request.top,
                request.left + request.width, request.top + request.height));
        _statusStripDirty = _statusStripUpload.IsValid();
    }
    return result;
}

void CustomSubBackground::RestoreStatusStrip()
{
    if (_statusStripAvailable && status_background::RebuildCustomStrip(_pristineStrip, _composedStrip))
    {
        _statusStripUpload = status_background::MergeStatusStripUploadPlans(
            _statusStripUpload, _publishedStatusStripUpload);
        _statusStripDirty = _statusStripUpload.IsValid();
    }
}
