#include "common.h"
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
#include "Pani.h"
#include "StartupIntro.h"

namespace
{
const InputKey SkipKeys = InputKey::A | InputKey::B | InputKey::Select | InputKey::Start |
    InputKey::DpadRight | InputKey::DpadLeft | InputKey::DpadUp | InputKey::DpadDown |
    InputKey::R | InputKey::L | InputKey::X | InputKey::Y | InputKey::Touch;

void LoadFrame(const u8* data, const startup_intro::PaniPackage& package, u16 frame, int background)
{
    dma_ntrCopy32(3, data + package.frames[frame].offset,
        bgGetGfxPtr(background), startup_intro::PaniFrameBytes);
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

    std::unique_ptr<u8[]> data(new(cache_align) u8[fileSize]);
    if (!data)
        return false;
    u32 bytesRead = 0;
    if (file.Read(data.get(), fileSize, bytesRead) != FR_OK || bytesRead != fileSize)
        return false;

    startup_intro::PaniPackage package;
    if (!startup_intro::ParsePani(data.get(), fileSize, package))
        return false;

    DC_FlushRange(data.get(), fileSize);
    sys_setMainEngineToBottomScreen();
    REG_MASTER_BRIGHT = 0x4010;
    REG_MASTER_BRIGHT_SUB = 0x4010;
    videoSetModeSub(MODE_5_2D);
    const int backgrounds[2] =
    {
        bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0),
        bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0)
    };
    bgHide(backgrounds[0]);
    bgHide(backgrounds[1]);
    LoadFrame(data.get(), package, 0, backgrounds[0]);
    dma_ntrCopy32(3, data.get() + package.paletteOffset, GFX_PLTT_BG_SUB, 256 * sizeof(u16));

    inputProvider.Reset();
    VBlank::Wait();
    bgShow(backgrounds[0]);
    REG_MASTER_BRIGHT_SUB = 0;
    SampleInput(inputProvider);
    bool skipArmed = (inputProvider.GetCurrentKeys() & SkipKeys) == InputKey::None;

    u16 currentFrame = 0;
    if (package.frameCount > 1)
        LoadFrame(data.get(), package, 1, backgrounds[1]);

    for (u16 tick = 1; tick <= package.totalDuration; tick++)
    {
        VBlank::Wait();
        SampleInput(inputProvider);
        const InputKey currentKeys = inputProvider.GetCurrentKeys() & SkipKeys;
        if (!skipArmed)
            skipArmed = currentKeys == InputKey::None;
        else if ((inputProvider.GetTriggeredKeys() & SkipKeys) != InputKey::None)
            break;

        if (tick == package.totalDuration)
            break;

        const u16 nextFrame = startup_intro::PaniFrameAtTick(package, tick);
        if (nextFrame != currentFrame)
        {
            bgHide(backgrounds[currentFrame & 1]);
            bgShow(backgrounds[nextFrame & 1]);
            currentFrame = nextFrame;
            if (nextFrame + 1 < package.frameCount)
                LoadFrame(data.get(), package, nextFrame + 1, backgrounds[(nextFrame + 1) & 1]);
        }
    }

    REG_MASTER_BRIGHT_SUB = 0x4010;
    bgHide(backgrounds[0]);
    bgHide(backgrounds[1]);
    while ((inputProvider.GetCurrentKeys() & SkipKeys) != InputKey::None)
    {
        VBlank::Wait();
        SampleInput(inputProvider);
    }
    inputProvider.Reset();
    return true;
}
