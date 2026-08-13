#pragma once
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

constexpr Color RailColor{ 0x3f, 0x48, 0x54 };
constexpr Color ProfileBackground{ 0x56, 0x61, 0x6e };
constexpr Color White{ 0xff, 0xff, 0xff };
constexpr Color BatteryGreen{ 0x6f, 0xdc, 0x50 };
constexpr Color OnlineBlue{ 0x66, 0xc0, 0xf4 };

constexpr uint8_t ProfileBackgroundPaletteIndex = 12;
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
    Bounds rail;
    Bounds volume;
    Bounds speaker;
    Bounds center;
    Bounds dateTime;
    Bounds battery;
    Bounds ntrText;
    Bounds profile;
    Bounds nickname;
    Bounds profileAvatar;
    Bounds profileContent;
    Bounds profileTrailingEdge;
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

constexpr uint8_t RailOamEntries = 8;
constexpr uint16_t RailGraphicsOffset = 0;
constexpr uint16_t RailGraphicsBytes = 256;
constexpr uint16_t SpeakerGraphicsOffset = 256;
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
constexpr uint16_t ProfileGraphicsOffset = BatteryGraphicsOffset + BatteryGraphicsBytes;
constexpr uint16_t ProfileGraphicsBytes = 128;
constexpr uint16_t StatusGraphicsBytes = ProfileGraphicsOffset + ProfileGraphicsBytes;
constexpr uint8_t NtrBatteryFrame = 10;
constexpr uint8_t MaxStatusOamEntries = 16;
constexpr uint16_t MaxStatusObjVramBytes = 2560;
constexpr uint8_t StatusPaletteRows = 4;
constexpr uint8_t NicknameCharacterLimit = 10;
constexpr uint8_t NicknameTextWidth = 50;

constexpr bool StatusGraphicsBlockFits(uint32_t offset)
{
    return (offset & 127) == 0 && offset <= 16384 - StatusGraphicsBytes;
}

constexpr bool StatusGraphicsWritable(uint32_t offset, bool hasAddress)
{
    return hasAddress && StatusGraphicsBlockFits(offset);
}

constexpr uint8_t RailPaletteIndex(uint8_t, uint8_t)
{
    return 1;
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
    palette.fill(ToRgb555(RailColor));
    palette[0] = 0;
    palette[1] = ToRgb555(RailColor);
    palette[ProfileBackgroundPaletteIndex] = ToRgb555(ProfileBackground);
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

constexpr Layout MakeLayout(bool dsiMode)
{
    Bounds center = dsiMode ? Bounds{ 86, 0, 170, 16 } : Bounds{ 73, 0, 183, 16 };
    int dateTimeLeft = center.left;
    return {
        { 0, 0, 256, 16 },
        { 2, 0, 18, 16 },
        { 2, 0, 18, 16 },
        center,
        { dateTimeLeft, 0, dateTimeLeft + 64, 16 },
        { 170, 4, 186, 12 },
        dsiMode ? Bounds{ 0, 0, 0, 0 } : Bounds{ 144, 0, 168, 16 },
        { 188, 0, 256, 16 },
        { 186, 0, 236, 16 },
        { 240, 0, 256, 16 },
        { 240, 0, 254, 16 },
        { 254, 0, 256, 16 },
        { 82, 0, 146, 16 },
        { 150, 0, 174, 16 }
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
