#include <cassert>
#include <fstream>
#include <string_view>

#include "romBrowser/views/StatusBarFormat.h"

template <typename T>
concept HasVolumeLevel = requires(T value) { value.volumeLevel; };

template <typename T>
concept HasVolumeText = requires(T value) { value.volumeText; };

template <typename T>
concept HasVolumeValue = requires(T value) { value.volume; };

template <typename T>
concept HasSpeakerFrame = requires(T value) { value.speakerFrame; };

std::string ReadFile(const char* path)
{
    std::ifstream stream(path);
    return { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
}

int main()
{
    using namespace status_bar;

    assert(BcdToInt(0x59) == 59);
    assert(NormalizeHour(0x12, false, false) == 0);
    assert(NormalizeHour(0x12, false, true) == 12);
    assert(NormalizeHour(0x09, false, true) == 21);
    assert(BatterySegments(0) == 0);
    assert(BatterySegments(3) == 1);
    assert(BatterySegments(15) == 4);

    static_assert(!HasVolumeLevel<Snapshot>);
    static_assert(!HasVolumeText<Layout>);
    static_assert(!HasVolumeValue<PresentationState>);
    static_assert(HasSpeakerFrame<PresentationState>);

    assert(SpeakerFrameForVolume(-1) == 0);
    assert(SpeakerFrameForVolume(0) == 0);
    assert(SpeakerFrameForVolume(1) == 1);
    assert(SpeakerFrameForVolume(10) == 1);
    assert(SpeakerFrameForVolume(11) == 2);
    assert(SpeakerFrameForVolume(20) == 2);
    assert(SpeakerFrameForVolume(21) == 3);
    assert(SpeakerFrameForVolume(31) == 3);
    assert(SpeakerFrameForVolume(32) == 3);
    assert(SpeakerFrameForVolume(255) == 3);

    Snapshot dsi{ 0x08, 0x11, 0x16, 0x42, 9, true, false, true, true };
    assert(std::string_view(FormatCenter(dsi).data()) == "08/11 16:42");

    Snapshot dsLow{ 0x08, 0x11, 0x09, 0x05, 3, false, true, false, false };
    assert(std::string_view(FormatCenter(dsLow).data()) == "08/11 21:05");
    dsLow.batteryLevel = 15;
    assert(std::string_view(FormatCenter(dsLow).data()) == "08/11 21:05");

    auto dsiPresentation = MakePresentation(dsi, true, 21);
    assert(dsiPresentation.volumeVisible);
    assert(dsiPresentation.speakerFrame == 3);
    assert(dsiPresentation.batteryFrame == 8);
    assert(!dsiPresentation.ntrBattery);

    auto unavailableVolume = MakePresentation(dsi, false, 11);
    assert(!unavailableVolume.volumeVisible);
    assert(unavailableVolume.speakerFrame == 2);

    for (int volume : { 0, 1, 10, 11, 20, 21, 31 })
    {
        auto presentation = MakePresentation(dsi, true, volume);
        assert(presentation.volumeVisible);
        assert(presentation.speakerFrame == SpeakerFrameForVolume(volume));
        assert(std::string_view(FormatCenter(dsi).data()) == "08/11 16:42");
    }

    for (uint8_t segments = 0; segments <= 4; segments++)
    {
        assert(DsiBatteryFrame(segments, false) == segments);
        assert(DsiBatteryFrame(segments, true) == segments + 5);
    }
    assert(NtrBatteryFrame == 10);

    auto ntrLowPresentation = MakePresentation(dsLow, true, 31);
    assert(ntrLowPresentation.batteryFrame == NtrBatteryFrame);
    assert(!ntrLowPresentation.volumeVisible);
    assert(ntrLowPresentation.speakerFrame == 3);
    assert(std::string_view(FormatBatteryFallback(ntrLowPresentation).data()) == "HIGH");
    dsLow.batteryLevel = 3;
    assert(std::string_view(FormatBatteryFallback(MakePresentation(dsLow, false, 0)).data()) == "LOW");

    static_assert(RailColor == Color{ 0x3f, 0x48, 0x54 });
    static_assert(ProfileBackground == Color{ 0x56, 0x61, 0x6e });
    static_assert(White == Color{ 0xff, 0xff, 0xff });
    static_assert(BatteryGreen == Color{ 0x6f, 0xdc, 0x50 });
    static_assert(OnlineBlue == Color{ 0x66, 0xc0, 0xf4 });
    static_assert(ToRgb555(RailColor) == 0x2928);
    static_assert(ToRgb555(ProfileBackground) == 0x358a);
    static_assert(ToRgb555(White) == 0x7fff);
    static_assert(ToRgb555(BatteryGreen) == 0x2b6d);
    static_assert(ToRgb555(OnlineBlue) == 0x7aec);
    static_assert(ProfileBackgroundPaletteIndex == 12);
    static_assert(OnlineBluePaletteIndex == 13);
    static_assert(BatteryGreenPaletteIndex == 14);
    static_assert(WhitePaletteIndex == 15);
    constexpr auto palette = MakeStatusPalette();
    static_assert(palette[0] == 0);
    static_assert(palette[1] == 0x2928);
    static_assert(palette[8] == palette[1]);
    static_assert(palette[9] == palette[1]);
    static_assert(palette[10] == palette[1]);
    static_assert(palette[11] == palette[1]);
    static_assert(palette[ProfileBackgroundPaletteIndex] == 0x358a);
    static_assert(palette[OnlineBluePaletteIndex] == 0x7aec);
    static_assert(palette[BatteryGreenPaletteIndex] == 0x2b6d);
    static_assert(palette[WhitePaletteIndex] == 0x7fff);
    constexpr std::array semanticPaletteIndices{
        ProfileBackgroundPaletteIndex, OnlineBluePaletteIndex, BatteryGreenPaletteIndex, WhitePaletteIndex,
    };
    for (size_t left = 0; left < semanticPaletteIndices.size(); left++)
    {
        assert(palette[semanticPaletteIndices[left]] != palette[1]);
        for (size_t right = left + 1; right < semanticPaletteIndices.size(); right++)
            assert(palette[semanticPaletteIndices[left]] != palette[semanticPaletteIndices[right]]);
    }
    constexpr auto dsiLayout = MakeLayout(true);
    constexpr auto ntrLayout = MakeLayout(false);
    static_assert(dsiLayout.rail == Bounds{ 0, 0, 256, 16 });
    static_assert(dsiLayout.volume == dsiLayout.speaker);
    static_assert(dsiLayout.speaker == Bounds{ 2, 0, 18, 16 });
    static_assert(dsiLayout.center == Bounds{ 86, 0, 170, 16 });
    static_assert(dsiLayout.dateTime == Bounds{ 86, 0, 150, 16 });
    static_assert(dsiLayout.battery == Bounds{ 170, 4, 186, 12 });
    static_assert(ntrLayout.center == Bounds{ 73, 0, 183, 16 });
    static_assert(ntrLayout.dateTime == Bounds{ 73, 0, 137, 16 });
    static_assert(ntrLayout.battery == Bounds{ 170, 4, 186, 12 });
    static_assert(ntrLayout.ntrText == Bounds{ 144, 0, 168, 16 });
    static_assert(dsiLayout.ntrText == Bounds{ 0, 0, 0, 0 });
    static_assert(dsiLayout.profile == Bounds{ 188, 0, 256, 16 });
    static_assert(dsiLayout.nickname == Bounds{ 186, 0, 236, 16 });
    static_assert(dsiLayout.profileAvatar == Bounds{ 240, 0, 256, 16 });
    static_assert(dsiLayout.profileContent == Bounds{ 240, 0, 254, 16 });
    static_assert(dsiLayout.profileTrailingEdge == Bounds{ 254, 0, 256, 16 });
    static_assert(dsiLayout.battery.left - dsiLayout.dateTime.right == 20);
    static_assert(dsiLayout.nickname.left == dsiLayout.battery.right);
    static_assert(dsiLayout.nickname.right - dsiLayout.nickname.left == NicknameTextWidth);
    static_assert(ntrLayout.battery.left - ntrLayout.ntrText.right == 2);
    static_assert(dsiLayout.fallbackDateTime == Bounds{ 82, 0, 146, 16 });
    static_assert(dsiLayout.fallbackBatteryState == Bounds{ 150, 0, 174, 16 });
    static_assert(ntrLayout.fallbackDateTime == dsiLayout.fallbackDateTime);
    static_assert(ntrLayout.fallbackBatteryState == dsiLayout.fallbackBatteryState);

    const auto viewSource = ReadFile("arm9/source/romBrowser/views/RomBrowserTopScreenView.cpp");
    assert(viewSource.find("OamBuilder::OamWithSize<16, 8>(") != std::string::npos);

    static_assert(!Overlaps(dsiLayout.volume, dsiLayout.center));
    static_assert(!Overlaps(dsiLayout.center, dsiLayout.profile));
    static_assert(!Overlaps(dsiLayout.nickname, dsiLayout.profileAvatar));
    static_assert(!Overlaps(ntrLayout.volume, ntrLayout.center));
    static_assert(!Overlaps(ntrLayout.center, ntrLayout.profile));
    static_assert(!Overlaps(dsiLayout.dateTime, dsiLayout.battery));
    static_assert(!Overlaps(ntrLayout.dateTime, ntrLayout.battery));
    static_assert(!Overlaps(ntrLayout.battery, ntrLayout.ntrText));
    static_assert(!Overlaps(ntrLayout.ntrText, ntrLayout.profile));

    assert(NicknameGlyph(u'A', true) == u'A');
    assert(NicknameGlyph(u'\u03A9', false) == u'?');
    assert(NicknameCharacterLimit == 10);
    static_assert(NicknameTextWidth == 50);
    assert(NicknameNeedsEllipsis(NicknameCharacterLimit + 1));
    assert(!NicknameNeedsEllipsis(NicknameCharacterLimit));

    static_assert(RailOamEntries == 8);
    static_assert(RailGraphicsOffset == 0);
    static_assert(RailGraphicsBytes == 256);
    static_assert(SpeakerGraphicsOffset == 256);
    static_assert(SpeakerFrameBytes == 128);
    static_assert(SpeakerFrameCount == 4);
    static_assert(SpeakerGraphicsBytes == 512);
    static_assert(SpeakerGraphicsOffset + 0 * SpeakerFrameBytes == 256);
    static_assert(SpeakerGraphicsOffset + 1 * SpeakerFrameBytes == 384);
    static_assert(SpeakerGraphicsOffset + 2 * SpeakerFrameBytes == 512);
    static_assert(SpeakerGraphicsOffset + 3 * SpeakerFrameBytes == 640);
    static_assert(BatteryGraphicsOffset == 768);
    static_assert(BatteryGraphicsBytes == 128);
    static_assert(BatterySpriteWidth == 16);
    static_assert(BatterySpriteHeight == 8);
    static_assert(BatteryFrameCount == 11);
    static_assert(BatteryFrameBytes == 64);
    static_assert(BatterySourceGraphicsBytes == 704);
    static_assert(BatteryGraphicsBytes == 2 * BatteryFrameBytes);
    static_assert(ProfileGraphicsOffset == 896);
    static_assert(ProfileGraphicsBytes == 128);
    static_assert(StatusGraphicsBytes == 1024);
    static_assert(MaxStatusOamEntries == 16);
    static_assert(MaxStatusObjVramBytes == 2560);
    static_assert(StatusPaletteRows == 4);
    assert(StatusGraphicsBlockFits(0));
    assert(StatusGraphicsBlockFits(16384 - StatusGraphicsBytes));
    assert(!StatusGraphicsBlockFits(1));
    assert(!StatusGraphicsBlockFits(16384 - StatusGraphicsBytes + 1));
    assert(StatusGraphicsWritable(0, true));
    assert(!StatusGraphicsWritable(1, true));
    assert(!StatusGraphicsWritable(0, false));
    for (uint8_t y = 0; y < 16; y++)
    {
        assert(RailPaletteIndex(0, y) == 1);
        assert(RailPaletteIndex(31, y) == 1);
    }

    assert(std::string_view(FormatBatteryFallback(dsiPresentation).data()) == "3/4+");
    dsiPresentation.batteryFrame = DsiBatteryFrame(0, false);
    assert(std::string_view(FormatBatteryFallback(dsiPresentation).data()) == "0/4");
    assert(std::string_view(FormatBatteryFallback(ntrLowPresentation).data()) == "HIGH");
    dsLow.batteryLevel = 3;
    assert(std::string_view(FormatBatteryFallback(MakePresentation(dsLow, false, 0)).data()) == "LOW");
}
