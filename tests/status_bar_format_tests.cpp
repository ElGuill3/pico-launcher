#include <cassert>
#include <fstream>
#include <string_view>

#include "gui/views/GlyphCoverage.h"
#include "gui/views/Nft2CoverageRenderer.h"
#include "romBrowser/views/StatusBarFormat.h"
#include "themes/background/StatusStripComposition.h"

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

    static_assert(White == Color{ 0xff, 0xff, 0xff });
    static_assert(BatteryGreen == Color{ 0x6f, 0xdc, 0x50 });
    static_assert(OnlineBlue == Color{ 0x66, 0xc0, 0xf4 });
    static_assert(StatusText == Color{ 0xd0, 0xd0, 0xd0 });
    static_assert(NicknameText == Color{ 0x4f, 0x9b, 0xc4 });
    static_assert(ToRgb555(White) == 0x7fff);
    static_assert(ToRgb555(BatteryGreen) == 0x2b6d);
    static_assert(ToRgb555(OnlineBlue) == 0x7aec);
    static_assert(ToRgb555(StatusText) == 0x6739);
    static_assert(ToRgb555(NicknameText) == 0x626a);
    static_assert(OnlineBluePaletteIndex == 13);
    static_assert(BatteryGreenPaletteIndex == 14);
    static_assert(WhitePaletteIndex == 15);
    static_assert(StatusNicknameRight == 256);
    static_assert(StatusBatteryGap == 18);
    constexpr auto palette = MakeStatusPalette();
    static_assert(palette[0] == 0);
    static_assert(palette[1] == 0);
    static_assert(palette[12] == 0);
    static_assert(palette[OnlineBluePaletteIndex] == 0x7aec);
    static_assert(palette[BatteryGreenPaletteIndex] == 0x2b6d);
    static_assert(palette[WhitePaletteIndex] == 0x7fff);
    constexpr std::array semanticPaletteIndices{
        OnlineBluePaletteIndex, BatteryGreenPaletteIndex, WhitePaletteIndex,
    };
    for (size_t left = 0; left < semanticPaletteIndices.size(); left++)
    {
        assert(palette[semanticPaletteIndices[left]] != 0);
        for (size_t right = left + 1; right < semanticPaletteIndices.size(); right++)
            assert(palette[semanticPaletteIndices[left]] != palette[semanticPaletteIndices[right]]);
    }
    constexpr auto dsiLayout = MakeLayout(true);
    constexpr auto ntrLayout = MakeLayout(false);
    static_assert(dsiLayout.volume == dsiLayout.speaker);
    static_assert(dsiLayout.speaker == Bounds{ 2, 0, 18, 16 });
    static_assert(dsiLayout.center == Bounds{ 96, 0, 160, 16 });
    static_assert(dsiLayout.dateTime == Bounds{ 96, 0, 160, 16 });
    static_assert(dsiLayout.battery == Bounds{ 160, 4, 176, 12 });
    static_assert(ntrLayout.center == Bounds{ 96, 0, 160, 16 });
    static_assert(ntrLayout.dateTime == Bounds{ 96, 0, 160, 16 });
    static_assert(ntrLayout.battery == Bounds{ 168, 4, 184, 12 });
    static_assert(ntrLayout.ntrText == Bounds{ 144, 0, 168, 16 });
    static_assert(dsiLayout.ntrText == Bounds{ 0, 0, 0, 0 });
    static_assert(dsiLayout.nickname == Bounds{ 190, 0, 256, 16 });
    static_assert(dsiLayout.battery.left - dsiLayout.dateTime.right == 0);
    static_assert(dsiLayout.nickname.left - dsiLayout.battery.right == 14);
    static_assert(dsiLayout.nickname.right - dsiLayout.nickname.left == NicknameTextWidth);
    static_assert(dsiLayout.fallbackDateTime == Bounds{ 96, 0, 160, 16 });
    static_assert(dsiLayout.fallbackBatteryState == Bounds{ 152, 0, 176, 16 });
    static_assert(ntrLayout.fallbackDateTime == dsiLayout.fallbackDateTime);
    static_assert(ntrLayout.fallbackBatteryState == Bounds{ 160, 0, 184, 16 });
    static_assert(!Overlaps(dsiLayout.nickname, dsiLayout.battery));
    static_assert(!Overlaps(ntrLayout.nickname, ntrLayout.ntrText));

    constexpr auto statusFont = StatusTextFontMetrics();
    static_assert(statusFont.ascend == 11 && statusFont.descend == 3);
    static_assert(StatusTextBaseline(statusFont, 16) == 1);
    static_assert(StatusTextInkTop(statusFont, 3, 16) == 4);
    static_assert(StatusTextInkBottom(statusFont, 9, 3, 16) == 13);
    static_assert(StatusTextWidth(StatusTextSample::DateTime) == 49);
    static_assert(StatusTextWidth(StatusTextSample::NtrHigh) == 23);
    static_assert(StatusTextWidth(StatusTextSample::NtrLow) == 20);
    static_assert(StatusTextWidth(StatusTextSample::NicknameEllipsis) == 53);
    static_assert(StatusTextFits(StatusTextSample::DateTime, dsiLayout.dateTime));
    static_assert(StatusTextFits(StatusTextSample::NtrHigh, ntrLayout.ntrText));
    static_assert(StatusTextFits(StatusTextSample::NtrLow, ntrLayout.ntrText));
    static_assert(dsiLayout.nickname == Bounds{ 190, 0, 256, 16 });
    static_assert(StatusTextFits(StatusTextSample::NicknameEllipsis, dsiLayout.nickname));
    static_assert(dsiLayout.nickname.left - dsiLayout.battery.right == 14);
    static_assert(!Overlaps(dsiLayout.nickname, dsiLayout.battery));

    constexpr auto shortNicknameInk = MakeNicknameInkBounds(10);
    constexpr auto maximumNicknameInk = MakeNicknameInkBounds(NicknameTextWidth);
    constexpr auto emptyNicknameInk = MakeNicknameInkBounds(0);
    constexpr auto overlongNicknameInk = MakeNicknameInkBounds(NicknameTextWidth + 1);
    static_assert(dsiLayout.dateTime == Bounds{ 96, 0, 160, 16 });
    static_assert(ntrLayout.dateTime == dsiLayout.dateTime);
    static_assert((dsiLayout.dateTime.left + dsiLayout.dateTime.right) / 2 == 128);
    static_assert(dsiLayout.speaker.top == 0 && dsiLayout.speaker.bottom == 16);
    static_assert(dsiLayout.battery.top == 4 && dsiLayout.battery.bottom == 12);
    static_assert(dsiLayout.nickname.right == StatusScreenWidth);
    static_assert(shortNicknameInk == Bounds{ 246, 0, 256, 16 });
    static_assert(maximumNicknameInk == Bounds{ 190, 0, 256, 16 });
    static_assert(emptyNicknameInk == Bounds{ 256, 0, 256, 16 });
    static_assert(overlongNicknameInk == maximumNicknameInk);
    static_assert(MakeBatteryBounds(true, 10) == Bounds{ 212, 4, 228, 12 });
    static_assert(MakeBatteryBounds(true, NicknameTextWidth) == Bounds{ 160, 4, 176, 12 });
    static_assert(MakeBatteryBounds(true, 0) == Bounds{ 222, 4, 238, 12 });
    static_assert(MakeBatteryBounds(true, NicknameTextWidth + 1) == MakeBatteryBounds(true, NicknameTextWidth));
    static_assert(shortNicknameInk.right == dsiLayout.nickname.right);
    static_assert(maximumNicknameInk.right == dsiLayout.nickname.right);
    static_assert(emptyNicknameInk.right == dsiLayout.nickname.right);
    static_assert(overlongNicknameInk.right == dsiLayout.nickname.right);
    static_assert(shortNicknameInk.left - MakeBatteryBounds(true, 10).right == 18);
    static_assert(maximumNicknameInk.left - MakeBatteryBounds(true, NicknameTextWidth).right == 14);
    static_assert(emptyNicknameInk.left - MakeBatteryBounds(true, 0).right == 18);
    static_assert(overlongNicknameInk.left - MakeBatteryBounds(true, NicknameTextWidth + 1).right == 14);
    static_assert(!Overlaps(dsiLayout.dateTime, MakeBatteryBounds(true, 10)));
    static_assert(!Overlaps(dsiLayout.dateTime, MakeBatteryBounds(true, NicknameTextWidth)));
    static_assert(!Overlaps(dsiLayout.dateTime, MakeBatteryBounds(true, 0)));
    static_assert(!Overlaps(dsiLayout.dateTime, MakeBatteryBounds(true, NicknameTextWidth + 1)));
    constexpr auto ntrShortBattery = MakeBatteryBounds(false, 10);
    constexpr auto ntrMaximumBattery = MakeBatteryBounds(false, NicknameTextWidth);
    constexpr auto ntrOverlongBattery = MakeBatteryBounds(false, NicknameTextWidth + 1);
    static_assert(ntrShortBattery == Bounds{ 212, 4, 228, 12 });
    static_assert(ntrMaximumBattery == Bounds{ 168, 4, 184, 12 });
    static_assert(ntrOverlongBattery == ntrMaximumBattery);
    static_assert(shortNicknameInk.left - ntrShortBattery.right == 18);
    static_assert(maximumNicknameInk.left - ntrMaximumBattery.right == 6);
    static_assert(!Overlaps(ntrLayout.ntrText, ntrShortBattery));
    static_assert(!Overlaps(ntrLayout.ntrText, ntrMaximumBattery));
    static_assert(!Overlaps(ntrLayout.ntrText, ntrOverlongBattery));
    static_assert(ntrMaximumBattery.left >= 0 && ntrMaximumBattery.right <= StatusScreenWidth);
    static_assert(ntrOverlongBattery.left >= 0 && ntrOverlongBattery.right <= StatusScreenWidth);
    static_assert(MakeBatteryBounds(true, 0).left >= 0 && MakeBatteryBounds(true, 0).right <= StatusScreenWidth);
    static_assert(MakeBatteryBounds(true, 10).left >= 0 && MakeBatteryBounds(true, 10).right <= StatusScreenWidth);
    static_assert(MakeBatteryBounds(true, NicknameTextWidth).left >= 0 &&
        MakeBatteryBounds(true, NicknameTextWidth).right <= StatusScreenWidth);
    static_assert(ntrShortBattery.left >= 0 && ntrShortBattery.right <= StatusScreenWidth);

    const auto viewSource = ReadFile("arm9/source/romBrowser/views/RomBrowserTopScreenView.cpp");
    const auto fontTypeSource = ReadFile("arm9/source/themes/FontType.h");
    const auto fontRepositorySource = ReadFile("arm9/source/themes/DefaultFontRepository.cpp");
    assert(fontTypeSource.find("Medium9") != std::string::npos);
    assert(fontRepositorySource.find("NotoSansJP-Medium-9_nft2.h") != std::string::npos);
    assert(viewSource.find("FontType::Medium9") != std::string::npos);
    assert(viewSource.find("OamBuilder::OamWithSize<16, 8>(") != std::string::npos);
    assert(viewSource.find("MakeRailGraphics") == std::string::npos);
    assert(viewSource.find("RailOamEntries") == std::string::npos);
    assert(viewSource.find("statusProfile") == std::string::npos);
    assert(viewSource.find("SetGlyphAntialiasing(antialias)") != std::string::npos);
    assert(viewSource.find("status_bar::StatusText.r, status_bar::StatusText.g, status_bar::StatusText.b") != std::string::npos);
    assert(viewSource.find("status_bar::NicknameText.r, status_bar::NicknameText.g, status_bar::NicknameText.b") != std::string::npos);
    assert(viewSource.find("status_bar::White.r, status_bar::White.g, status_bar::White.b") == std::string::npos);
    assert(viewSource.find("status_bar::OnlineBlue.r, status_bar::OnlineBlue.g, status_bar::OnlineBlue.b") == std::string::npos);

    static_assert(glyph_coverage::BinaryThreshold == 8);
    for (uint8_t low = 0; low < 16; low++)
    {
        for (uint8_t high = 0; high < 16; high++)
        {
            const uint8_t source = low | (high << 4);
            const uint8_t expected = (low >= 8 ? 0x0f : 0) | (high >= 8 ? 0xf0 : 0);
            assert(glyph_coverage::QuantizeByte(source) == expected);
        }
    }
    static_assert(glyph_coverage::QuantizeByte(0x78) == 0x0f);
    static_assert(glyph_coverage::QuantizeByte(0x87) == 0xf0);
    static_assert(glyph_coverage::QuantizeByte(0x88) == 0xff);

    static_assert(!Overlaps(dsiLayout.volume, dsiLayout.center));
    static_assert(!Overlaps(ntrLayout.volume, ntrLayout.center));
    static_assert(!Overlaps(dsiLayout.dateTime, dsiLayout.battery));
    static_assert(!Overlaps(ntrLayout.dateTime, ntrLayout.battery));

    assert(NicknameGlyph(u'A', true) == u'A');
    assert(NicknameGlyph(u'\u03A9', false) == u'?');
    assert(NicknameCharacterLimit == 10);
    static_assert(NicknameTextWidth == 66);
    assert(NicknameNeedsEllipsis(NicknameCharacterLimit + 1));
    assert(!NicknameNeedsEllipsis(NicknameCharacterLimit));
    assert(status_strip::ScreenWidth == 256);
    assert(status_strip::ScreenHeight == 16);
    assert(Nft2CoverageRenderer::PreservesFullCoverage);

    static_assert(StatusTextMode(true, true, false) == TextMode::Composed);
    static_assert(StatusTextMode(true, false, true) == TextMode::MaterialObject);
    static_assert(StatusTextMode(true, false, false) == TextMode::BinaryFallback);
    static_assert(StatusTextMode(false, true, false) == TextMode::BinaryFallback);
    static_assert(StatusTextMode(false, false, true) == TextMode::BinaryFallback);
    static_assert(StatusTextRequiresPristineRestore(true, false));
    static_assert(!StatusTextRequiresPristineRestore(true, true));
    static_assert(!StatusTextRequiresPristineRestore(false, false));
    static_assert(!StatusTextVisible(TextMode::Composed, false));
    static_assert(!StatusTextVisible(TextMode::MaterialObject, false));
    static_assert(!StatusTextVisible(TextMode::Composed, true));
    static_assert(StatusTextVisible(TextMode::BinaryFallback, true));
    static_assert(StatusIconsVisible(TextMode::Composed, true));
    static_assert(StatusIconsVisible(TextMode::MaterialObject, true));
    static_assert(StatusIconsVisible(TextMode::BinaryFallback, true));
    static_assert(!StatusIconsVisible(TextMode::Composed, false));

    static_assert(SpeakerGraphicsOffset == 0);
    static_assert(SpeakerFrameBytes == 128);
    static_assert(SpeakerFrameCount == 4);
    static_assert(SpeakerGraphicsBytes == 512);
    static_assert(SpeakerGraphicsOffset + 0 * SpeakerFrameBytes == 0);
    static_assert(SpeakerGraphicsOffset + 1 * SpeakerFrameBytes == 128);
    static_assert(SpeakerGraphicsOffset + 2 * SpeakerFrameBytes == 256);
    static_assert(SpeakerGraphicsOffset + 3 * SpeakerFrameBytes == 384);
    static_assert(BatteryGraphicsOffset == 512);
    static_assert(BatteryGraphicsBytes == 128);
    static_assert(BatterySpriteWidth == 16);
    static_assert(BatterySpriteHeight == 8);
    static_assert(BatteryFrameCount == 11);
    static_assert(BatteryFrameBytes == 64);
    static_assert(BatterySourceGraphicsBytes == 704);
    static_assert(BatteryGraphicsBytes == 2 * BatteryFrameBytes);
    static_assert(StatusGraphicsBytes == 640);
    static_assert(MaxStatusOamEntries == 7);
    static_assert(StatusLabelObjVramBytes(64, 16) == 512);
    static_assert(StatusLabelObjVramBytes(24, 16) == 256);
    static_assert(StatusLabelObjVramBytes(NicknameTextWidth, 16) == 768);
    static_assert(MaxStatusObjVramBytes == StatusLabelObjVramBytes(64, 16) +
        StatusLabelObjVramBytes(24, 16) + StatusLabelObjVramBytes(NicknameTextWidth, 16) +
        StatusGraphicsBytes);
    static_assert(MaxStatusObjVramBytes == 2176);
    static_assert(StatusPaletteRows == 4);
    assert(StatusGraphicsBlockFits(0));
    assert(StatusGraphicsBlockFits(16384 - StatusGraphicsBytes));
    assert(!StatusGraphicsBlockFits(1));
    assert(!StatusGraphicsBlockFits(16384 - StatusGraphicsBytes + 1));
    assert(StatusGraphicsWritable(0, true));
    assert(!StatusGraphicsWritable(1, true));
    assert(!StatusGraphicsWritable(0, false));
    assert(std::string_view(FormatBatteryFallback(dsiPresentation).data()) == "3/4+");
    dsiPresentation.batteryFrame = DsiBatteryFrame(0, false);
    assert(std::string_view(FormatBatteryFallback(dsiPresentation).data()) == "0/4");
    assert(std::string_view(FormatBatteryFallback(ntrLowPresentation).data()) == "HIGH");
    dsLow.batteryLevel = 3;
    assert(std::string_view(FormatBatteryFallback(MakePresentation(dsLow, false, 0)).data()) == "LOW");

    const auto appSource = ReadFile("arm9/source/App.cpp");
    const auto customBackgroundSource = ReadFile("arm9/source/themes/custom/CustomSubBackground.cpp");
    assert(appSource.find("_topBackground.get(),") != std::string::npos);
    assert(appSource.find("_topBackground->VBlank();") < appSource.find("_romBrowserTopScreenView->VBlank();"));
    assert(viewSource.find("PrepareStatusStrip") != std::string::npos);
    assert(customBackgroundSource.find("AtomicStatusStripPublication") == std::string::npos);
    assert(customBackgroundSource.find("for (int y = _statusStripUpload.top") != std::string::npos);
    assert(customBackgroundSource.find("DC_FlushRange(_composedStrip.data() + offset, _statusStripUpload.RowBytes())") != std::string::npos);
    assert(viewSource.find("RestoreStatusStrip") != std::string::npos);
    assert(viewSource.find("Nft2CoverageRenderer::Render") != std::string::npos);
    assert(viewSource.find("FontType::Medium9") != std::string::npos);
    assert(viewSource.find("StatusTextBaseline") != std::string::npos);
    assert(viewSource.find("TextMode::BinaryFallback") != std::string::npos);
    assert(viewSource.find("ComposedBackground && _statusGraphicsReady") != std::string::npos);
    assert(viewSource.find("UniformObject && _statusGraphicsReady") != std::string::npos);
    assert(viewSource.find("StatusTextMode(_statusGraphicsReady, false, true)") != std::string::npos);
    assert(viewSource.find("StatusTextRequiresPristineRestore(true, composed)") != std::string::npos);
    assert(viewSource.find("_materialStatusPalettes.data() + 16") != std::string::npos);
    assert(viewSource.find("GetRenderedStringWidth") != std::string::npos);
    assert(viewSource.find("MakeBatteryBounds(snapshot.dsiMode, nicknameWidth)") != std::string::npos);
}
