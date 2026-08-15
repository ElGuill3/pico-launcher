#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

namespace status_bar
{
struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    constexpr bool operator==(const Color&) const = default;
};

constexpr Color White{ 0xff, 0xff, 0xff };
constexpr Color BatteryGreen{ 0x6f, 0xdc, 0x50 };
constexpr Color OnlineBlue{ 0x66, 0xc0, 0xf4 };
constexpr Color StatusText{ 0xd0, 0xd0, 0xd0 };
constexpr Color NicknameText{ 0x4f, 0x9b, 0xc4 };

constexpr uint8_t OnlineBluePaletteIndex = 13;
constexpr uint8_t BatteryGreenPaletteIndex = 14;
constexpr uint8_t WhitePaletteIndex = 15;

struct Bounds
{
    int left;
    int top;
    int right;
    int bottom;

    constexpr bool operator==(const Bounds&) const = default;
};

struct Layout
{
    Bounds volume;
    Bounds speaker;
    Bounds center;
    Bounds dateTime;
    Bounds battery;
    Bounds ntrText;
    Bounds nickname;
    Bounds fallbackDateTime;
    Bounds fallbackBatteryState;
};

struct Snapshot
{
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t batteryLevel;
    bool is24Hour;
    bool pm;
    bool dsiMode;
    bool charging;
};

using CenterText = std::array<char, 32>;
using BatteryText = std::array<char, 6>;

struct PresentationState
{
    bool volumeVisible;
    uint8_t speakerFrame;
    uint8_t batteryFrame;
    bool ntrBattery;
    bool ntrHigh;
};

constexpr uint16_t SpeakerGraphicsOffset = 0;
constexpr uint16_t SpeakerFrameBytes = 128;
constexpr uint8_t SpeakerFrameCount = 4;
constexpr uint16_t SpeakerGraphicsBytes = SpeakerFrameBytes * SpeakerFrameCount;
constexpr uint16_t BatteryGraphicsOffset = SpeakerGraphicsOffset + SpeakerGraphicsBytes;
constexpr uint16_t BatteryGraphicsBytes = 128;
constexpr uint8_t BatterySpriteWidth = 16;
constexpr uint8_t BatterySpriteHeight = 8;
constexpr uint8_t BatteryFrameCount = 11;
constexpr uint8_t BatteryFrameBytes = 64;
constexpr uint16_t BatterySourceGraphicsBytes = BatteryFrameCount * BatteryFrameBytes;
constexpr uint16_t StatusGraphicsBytes = BatteryGraphicsOffset + BatteryGraphicsBytes;
constexpr uint8_t NtrBatteryFrame = 10;
constexpr uint8_t MaxStatusOamEntries = 7;
constexpr uint16_t StatusLabelObjVramBytes(uint16_t width, uint16_t height)
{
    const uint16_t roundedWidth = (width + 31) & ~31;
    const uint16_t roundedHeight = (height + 15) & ~15;
    return (roundedWidth * roundedHeight) / 2;
}
constexpr uint16_t MaxStatusObjVramBytes = StatusLabelObjVramBytes(64, 16) +
    StatusLabelObjVramBytes(24, 16) + StatusLabelObjVramBytes(66, 16) + StatusGraphicsBytes;
constexpr uint8_t StatusPaletteRows = 4;
constexpr uint8_t NicknameCharacterLimit = 10;
constexpr uint8_t NicknameTextWidth = 66;
constexpr uint8_t StatusHeight = 16;
constexpr uint16_t StatusCoverageBytes = NicknameTextWidth * StatusHeight;
constexpr int StatusScreenWidth = 256;
constexpr int StatusVolumeLeft = 2;
constexpr int StatusNicknameRight = StatusScreenWidth;
constexpr int StatusDateTimeLeft = 96;
constexpr int StatusDateTimeRight = 160;
constexpr int StatusNtrTextRight = 168;
constexpr int StatusBatteryGap = 18;

struct FontMetrics
{
    int ascend;
    int descend;
};

enum class StatusTextSample
{
    DateTime,
    NtrHigh,
    NtrLow,
    NicknameEllipsis,
};

constexpr FontMetrics StatusTextFontMetrics()
{
    // NotoSansJP Medium 9: generated from the pinned source and inspected from NFT2.
    return { 11, 3 };
}

constexpr int StatusTextBaseline(FontMetrics metrics, int height)
{
    return (height - metrics.ascend - metrics.descend) / 2;
}

constexpr int StatusTextInkTop(FontMetrics metrics, int glyphSpacingTop, int height)
{
    return StatusTextBaseline(metrics, height) + glyphSpacingTop;
}

constexpr int StatusTextInkBottom(FontMetrics metrics, int glyphHeight, int glyphSpacingTop, int height)
{
    return StatusTextInkTop(metrics, glyphSpacingTop, height) + glyphHeight;
}

constexpr int StatusTextWidth(StatusTextSample sample)
{
    switch (sample)
    {
        case StatusTextSample::DateTime: return 49; // "12/31 23:59"
        case StatusTextSample::NtrHigh: return 23;
        case StatusTextSample::NtrLow: return 20;
        case StatusTextSample::NicknameEllipsis: return 53; // "WWWW ... W" in 66 pixels
    }
    return 0;
}

enum class TextMode
{
    Composed,
    MaterialObject,
    BinaryFallback,
};

constexpr TextMode StatusTextMode(bool statusGraphicsReady, bool customComposed, bool materialObject)
{
    if (!statusGraphicsReady)
        return TextMode::BinaryFallback;
    return customComposed ? TextMode::Composed : materialObject ? TextMode::MaterialObject : TextMode::BinaryFallback;
}

constexpr bool StatusTextRequiresPristineRestore(bool composedBackground, bool compositionComplete)
{
    return composedBackground && !compositionComplete;
}

constexpr bool StatusTextVisible(TextMode mode, bool binaryObject)
{
    return mode != TextMode::Composed && binaryObject;
}

constexpr bool StatusIconsVisible(TextMode, bool statusGraphicsReady)
{
    return statusGraphicsReady;
}

constexpr std::array<uint16_t, 16> MakeCoveragePalette(uint16_t background, uint16_t foreground)
{
    std::array<uint16_t, 16> palette{};
    for (uint16_t coverage = 0; coverage < palette.size(); ++coverage)
    {
        const auto blend = [coverage](uint16_t dst, uint16_t src)
        {
            return static_cast<uint16_t>((dst * (15 - coverage) + src * coverage + 7) / 15);
        };
        palette[coverage] = static_cast<uint16_t>((background & 0x8000) |
            blend(background & 0x1f, foreground & 0x1f) |
            (blend((background >> 5) & 0x1f, (foreground >> 5) & 0x1f) << 5) |
            (blend((background >> 10) & 0x1f, (foreground >> 10) & 0x1f) << 10));
    }
    return palette;
}

constexpr bool StatusGraphicsBlockFits(uint32_t offset)
{
    return (offset & 127) == 0 && offset <= 16384 - StatusGraphicsBytes;
}

constexpr bool StatusGraphicsWritable(uint32_t offset, bool hasAddress)
{
    return hasAddress && StatusGraphicsBlockFits(offset);
}

constexpr uint16_t ToRgb555(Color color)
{
    uint16_t r = (color.r * 31 + 127) / 255;
    uint16_t g = (color.g * 31 + 127) / 255;
    uint16_t b = (color.b * 31 + 127) / 255;
    return r | (g << 5) | (b << 10);
}

constexpr std::array<uint16_t, 16> MakeStatusPalette()
{
    std::array<uint16_t, 16> palette{};
    palette[OnlineBluePaletteIndex] = ToRgb555(OnlineBlue);
    palette[BatteryGreenPaletteIndex] = ToRgb555(BatteryGreen);
    palette[WhitePaletteIndex] = ToRgb555(White);
    return palette;
}

constexpr int BcdToInt(uint8_t value)
{
    return (value >> 4) * 10 + (value & 0x0F);
}

constexpr int NormalizeHour(uint8_t hour, bool is24Hour, bool pm)
{
    int value = BcdToInt(hour);
    if (!is24Hour)
    {
        if (value == 12)
            value = 0;
        if (pm)
            value += 12;
    }
    return value;
}

constexpr int BatterySegments(uint8_t level)
{
    return level == 0 ? 0 : (level + 3) / 4;
}

constexpr uint8_t DsiBatteryFrame(uint8_t segments, bool charging)
{
    uint8_t frame = segments > 4 ? 4 : segments;
    return frame + (charging ? 5 : 0);
}

constexpr uint8_t SpeakerFrameForVolume(int volumeLevel)
{
    if (volumeLevel <= 0)
        return 0;
    if (volumeLevel <= 10)
        return 1;
    if (volumeLevel <= 20)
        return 2;
    return 3;
}

constexpr PresentationState MakePresentation(const Snapshot& status, bool volumeValid, int volumeLevel)
{
    bool ntrBattery = !status.dsiMode;
    uint8_t batteryFrame = ntrBattery
        ? NtrBatteryFrame
        : DsiBatteryFrame(BatterySegments(status.batteryLevel > 15 ? 15 : status.batteryLevel), status.charging);
    return { status.dsiMode && volumeValid, SpeakerFrameForVolume(volumeLevel), batteryFrame,
        ntrBattery, status.batteryLevel > 3 };
}

constexpr bool Overlaps(const Bounds& first, const Bounds& second)
{
    return first.left < second.right && second.left < first.right &&
        first.top < second.bottom && second.top < first.bottom;
}

constexpr bool StatusTextFits(StatusTextSample sample, Bounds bounds)
{
    return StatusTextWidth(sample) <= bounds.right - bounds.left;
}

constexpr Bounds MakeNicknameInkBounds(int displayedWidth)
{
    const int width = std::clamp(displayedWidth, 0, static_cast<int>(NicknameTextWidth));
    return { StatusNicknameRight - width, 0, StatusNicknameRight, StatusHeight };
}

constexpr Bounds MakeBatteryBounds(bool dsiMode, int displayedNicknameWidth)
{
    const auto nickname = MakeNicknameInkBounds(displayedNicknameWidth);
    const int minimumLeft = dsiMode ? StatusDateTimeRight : StatusNtrTextRight;
    const int right = std::clamp(nickname.left - StatusBatteryGap,
        minimumLeft + BatterySpriteWidth, StatusNicknameRight);
    return { right - BatterySpriteWidth, 4, right, 12 };
}

constexpr Bounds MakeBatteryStateBounds(bool dsiMode, int displayedNicknameWidth)
{
    const auto battery = MakeBatteryBounds(dsiMode, displayedNicknameWidth);
    return { battery.right - 24, 0, battery.right, StatusHeight };
}

constexpr Layout MakeLayout(bool dsiMode)
{
    Bounds center{ StatusDateTimeLeft, 0, StatusDateTimeRight, StatusHeight };
    return {
        { StatusVolumeLeft, 0, StatusVolumeLeft + 16, 16 },
        { StatusVolumeLeft, 0, StatusVolumeLeft + 16, 16 },
        center,
        center,
        MakeBatteryBounds(dsiMode, NicknameTextWidth),
        dsiMode ? Bounds{ 0, 0, 0, 0 } : Bounds{ 144, 0, StatusNtrTextRight, 16 },
        { StatusNicknameRight - NicknameTextWidth, 0, StatusNicknameRight, 16 },
        center,
        MakeBatteryStateBounds(dsiMode, NicknameTextWidth)
    };
}

constexpr char16_t NicknameGlyph(char16_t glyph, bool supported)
{
    return supported ? glyph : u'?';
}

constexpr bool NicknameNeedsEllipsis(uint8_t length)
{
    return length > NicknameCharacterLimit;
}

inline void AppendTwoDigits(char*& output, int value)
{
    *output++ = '0' + value / 10;
    *output++ = '0' + value % 10;
}

inline void Append(char*& output, const char* text)
{
    while (*text)
        *output++ = *text++;
}

inline CenterText FormatCenter(const Snapshot& status)
{
    CenterText text{};
    char* output = text.data();
    AppendTwoDigits(output, BcdToInt(status.month));
    *output++ = '/';
    AppendTwoDigits(output, BcdToInt(status.day));
    *output++ = ' ';
    AppendTwoDigits(output, NormalizeHour(status.hour, status.is24Hour, status.pm));
    *output++ = ':';
    AppendTwoDigits(output, BcdToInt(status.minute));
    return text;
}

inline BatteryText FormatBatteryFallback(const PresentationState& presentation)
{
    BatteryText text{};
    char* output = text.data();
    if (presentation.ntrBattery)
    {
        Append(output, presentation.ntrHigh ? "HIGH" : "LOW");
        return text;
    }

    uint8_t frame = presentation.batteryFrame;
    uint8_t segments = frame >= 5 ? frame - 5 : frame;
    *output++ = '0' + segments;
    *output++ = '/';
    *output++ = '4';
    if (frame >= 5)
        *output++ = '+';
    return text;
}
}
