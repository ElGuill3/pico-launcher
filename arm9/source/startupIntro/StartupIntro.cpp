#include "common.h"
#include <cstring>
#include <memory>
#include <nds/arm9/background.h>
#include <nds/arm9/cache.h>
#include <libtwl/dma/dmaNitro.h>
#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxPalette.h>
#include <libtwl/sys/sysPower.h>
#include "core/mini-printf.h"
#include "fat/File.h"
#include "gui/input/SampledInputProvider.h"
#include "Brightness.h"
#include "Pani.h"
#include "StartupIntro.h"

namespace
{
const InputKey SkipKeys = InputKey::A | InputKey::B | InputKey::Select | InputKey::Start |
    InputKey::DpadRight | InputKey::DpadLeft | InputKey::DpadUp | InputKey::DpadDown |
    InputKey::R | InputKey::L | InputKey::X | InputKey::Y | InputKey::Touch;
constexpr u32 MetadataMaxBytes = 32 + 256 * sizeof(u16) + startup_intro::PaniMaxFrames * 12;

bool LoadFrame(File& file, const startup_intro::PaniPackage& package, u16 frame, int background,
    u8* compressed, u8* decoded)
{
    const startup_intro::PaniFrame& source = package.frames[frame];
    if (file.Seek(source.offset) != FR_OK)
        return false;

    if (package.codec == 0)
    {
        if (!file.ReadExact(decoded, source.length))
            return false;
    }
    else
    {
        if (!file.ReadExact(compressed, startup_intro::PaniLz4StoredBytes(source.length)) ||
            !startup_intro::DecodePaniFrame(package.codec, compressed, source.length,
                decoded, startup_intro::PaniFrameBytes))
            return false;
    }
    DC_FlushRange(decoded, startup_intro::PaniFrameBytes);
    dma_ntrCopy32(3, decoded, bgGetGfxPtr(background), startup_intro::PaniFrameBytes);
    return true;
}

void SampleInput(SampledInputProvider& inputProvider)
{
    inputProvider.Sample();
    inputProvider.Update();
}
}

bool TryPlayStartupIntro(const char* themeName, SampledInputProvider& inputProvider)
{
    TCHAR path[128];
    mini_snprintf(path, sizeof(path), "/_pico/themes/%s/intro.pani", themeName);

    File file;
    if (file.Open(path, FA_OPEN_EXISTING | FA_READ) != FR_OK)
        return false;

    const u64 fileSize = file.GetSize();
    if (fileSize < 32 || fileSize > startup_intro::PaniMaxPackageBytes)
        return false;

    const u32 metadataSize = fileSize < MetadataMaxBytes ? fileSize : MetadataMaxBytes;
    std::unique_ptr<u8[]> metadata(new(cache_align) u8[metadataSize]);
    if (!metadata || !file.ReadExact(metadata.get(), metadataSize))
        return false;

    auto package = std::make_unique<startup_intro::PaniPackage>();
    if (!package || !startup_intro::ParsePaniHeader(metadata.get(), metadataSize, fileSize, *package))
        return false;

    constexpr u32 PaletteBytes = 256 * sizeof(u16);
    if (package->paletteOffset > metadataSize ||
        PaletteBytes > metadataSize - package->paletteOffset)
        return false;
    const u32 tableSize = package->frameCount * 12;
    if (package->tableOffset > metadataSize || tableSize > metadataSize - package->tableOffset ||
        !startup_intro::ParsePaniFrameTable(metadata.get() + package->tableOffset,
            tableSize, fileSize, *package))
        return false;

    u8* palette = metadata.get() + package->paletteOffset;
    std::unique_ptr<u8[]> alignedPalette;
    if ((package->paletteOffset & 31) != 0)
    {
        alignedPalette.reset(new(cache_align) u8[PaletteBytes]);
        if (!alignedPalette)
            return false;
        std::memcpy(alignedPalette.get(), palette, PaletteBytes);
        palette = alignedPalette.get();
    }

    std::unique_ptr<u8[]> decoded(new(cache_align) u8[startup_intro::PaniFrameBytes]);
    std::unique_ptr<u8[]> compressed;
    if (package->codec == 1)
        compressed.reset(new(cache_align) u8[startup_intro::PaniFrameBytes]);
    if (!decoded || (package->codec == 1 && !compressed))
        return false;

    sys_setMainEngineToBottomScreen();
    GFX_PLTT_BG_MAIN[0] = startup_intro::MainBackdropColor;
    videoSetMode(startup_intro::MainBackdropDisplayMode);
    REG_MASTER_BRIGHT = startup_intro::HiddenBrightness;
    REG_MASTER_BRIGHT_SUB = startup_intro::HiddenBrightness;
    videoSetModeSub(MODE_5_2D);
    const int backgrounds[2] =
    {
        bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0),
        bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0)
    };
    bgHide(backgrounds[0]);
    bgHide(backgrounds[1]);
    if (!LoadFrame(file, *package, 0, backgrounds[0], compressed.get(), decoded.get()))
        return false;
    DC_FlushRange(palette, PaletteBytes);
    dma_ntrCopy32(3, palette, GFX_PLTT_BG_SUB, PaletteBytes);

    inputProvider.Reset();
    VBlank::Wait();
    bgShow(backgrounds[0]);
    REG_MASTER_BRIGHT_SUB = 0;
    SampleInput(inputProvider);
    bool skipArmed = (inputProvider.GetCurrentKeys() & SkipKeys) == InputKey::None;

    u16 currentFrame = 0;
    bool validPlayback = true;
    if (package->frameCount > 1 &&
        !LoadFrame(file, *package, 1, backgrounds[1], compressed.get(), decoded.get()))
        return false;

    for (u16 tick = 1; tick <= package->totalDuration; tick++)
    {
        VBlank::Wait();
        SampleInput(inputProvider);
        const InputKey currentKeys = inputProvider.GetCurrentKeys() & SkipKeys;
        if (!skipArmed)
            skipArmed = currentKeys == InputKey::None;
        else if ((inputProvider.GetTriggeredKeys() & SkipKeys) != InputKey::None)
            break;

        if (tick == package->totalDuration)
            break;

        const u16 nextFrame = startup_intro::PaniFrameAtTick(*package, tick);
        if (nextFrame != currentFrame)
        {
            bgHide(backgrounds[currentFrame & 1]);
            bgShow(backgrounds[nextFrame & 1]);
            currentFrame = nextFrame;
            if (nextFrame + 1 < package->frameCount &&
                !LoadFrame(file, *package, nextFrame + 1, backgrounds[(nextFrame + 1) & 1],
                    compressed.get(), decoded.get()))
            {
                validPlayback = false;
                break;
            }
        }
    }

    REG_MASTER_BRIGHT_SUB = startup_intro::HiddenBrightness;
    bgHide(backgrounds[0]);
    bgHide(backgrounds[1]);
    while ((inputProvider.GetCurrentKeys() & SkipKeys) != InputKey::None)
    {
        VBlank::Wait();
        SampleInput(inputProvider);
    }
    inputProvider.Reset();
    return validPlayback;
}
