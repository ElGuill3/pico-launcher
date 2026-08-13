#include "common.h"
#include <array>
#include <libtwl/mem/memVram.h>
#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxBackground.h>
#include <libtwl/gfx/gfxPalette.h>
#include <libtwl/gfx/gfxWindow.h>
#include <nds/system.h>
#include "gui/OamBuilder.h"
#include "gui/OamManager.h"
#include "gui/palette/DirectPalette.h"
#include "../viewModels/RomBrowserViewModel.h"
#include "rtcIpc.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "gui/VramContext.h"
#include "themes/IFontRepository.h"
#include "../Theme/IRomBrowserViewFactory.h"
#include "statusBattery.h"
#include "statusProfile.h"
#include "statusSpeaker.h"
#include "RomBrowserTopScreenView.h"

namespace
{
void MakeRailGraphics(std::array<uint8_t, status_bar::RailGraphicsBytes>& graphics)
{
    for (uint8_t y = 0; y < 16; y++)
    {
        for (uint8_t x = 0; x < 32; x += 2)
        {
            uint8_t tile = (y >> 3) * 4 + (x >> 3);
            uint8_t byteOffset = tile * 32 + (y & 7) * 4 + ((x & 7) >> 1);
            uint8_t color = status_bar::RailPaletteIndex(x, y);
            graphics[byteOffset] = color | (color << 4);
        }
    }
}
}

RomBrowserTopScreenView::RomBrowserTopScreenView(
    SharedPtr<RomBrowserViewModel> viewModel,
    const RomBrowserDisplayMode* displayMode,
    const IThemeFileIconFactory* themeFileIconFactory,
    const IRomBrowserViewFactory* romBrowserViewFactory,
    const MaterialColorScheme*,
    const IFontRepository* fontRepository)
    : _viewModel(std::move(viewModel))
    , _themeFileIconFactory(themeFileIconFactory)
    , _fileInfoView(romBrowserViewFactory->CreateFileInfoView())
    , _centerStatusLabel(Label2DView::CreateShared(64, 16, 11, fontRepository->GetFont(FontType::Medium7_5)))
    , _batteryStateLabel(Label2DView::CreateShared(24, 16, 4, fontRepository->GetFont(FontType::Medium7_5)))
    , _nicknameLabel(Label2DView::CreateShared(status_bar::NicknameTextWidth, 16, 10,
        fontRepository->GetFont(FontType::Medium7_5)))
    , _showCover(displayMode->ShowCoverOnTopScreen())
    , _coverPosition(romBrowserViewFactory->GetTopCoverPosition())
{
    AddChildTail(_fileInfoView.GetPointer());
    const Rgb<8, 8, 8> railColor{ status_bar::RailColor.r, status_bar::RailColor.g, status_bar::RailColor.b };
    const Rgb<8, 8, 8> foregroundColor{ status_bar::White.r, status_bar::White.g, status_bar::White.b };
    const Rgb<8, 8, 8> onlineColor{ status_bar::OnlineBlue.r, status_bar::OnlineBlue.g, status_bar::OnlineBlue.b };
    _statusPalette = status_bar::MakeStatusPalette();
    _statusLayout = status_bar::MakeLayout(isDSiMode());

    _centerStatusLabel->SetPosition(_statusLayout.dateTime.left, 0);
    _centerStatusLabel->SetHorizontalAlignment(Alignment::Center);
    _centerStatusLabel->SetVerticalAlignment(Alignment::Center);
    _centerStatusLabel->SetBackgroundColor(railColor);
    _centerStatusLabel->SetForegroundColor(foregroundColor);
    AddChildTail(_centerStatusLabel.GetPointer());

    _batteryStateLabel->SetPosition(_statusLayout.ntrText.left, 0);
    _batteryStateLabel->SetVerticalAlignment(Alignment::Center);
    _batteryStateLabel->SetBackgroundColor(railColor);
    _batteryStateLabel->SetForegroundColor(foregroundColor);
    AddChildTail(_batteryStateLabel.GetPointer());

    _nicknameLabel->SetPosition(_statusLayout.nickname.left, 0);
    _nicknameLabel->SetHorizontalAlignment(Alignment::End);
    _nicknameLabel->SetVerticalAlignment(Alignment::Center);
    _nicknameLabel->SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    _nicknameLabel->SetBackgroundColor(railColor);
    _nicknameLabel->SetForegroundColor(onlineColor);

    char16_t nickname[11];
    u32 nicknameLength = std::min<u32>(PersonalData->nameLen, 10);
    for (u32 i = 0; i < nicknameLength; i++)
    {
        u16 character = PersonalData->name[i];
        if (character == 0)
        {
            nicknameLength = i;
            break;
        }
        nickname[i] = status_bar::NicknameGlyph(character,
            nft2_findGlyphIdxForCharacter(fontRepository->GetFont(FontType::Medium7_5), character) != 0);
    }
    nickname[nicknameLength] = 0;
    _nicknameLabel->SetText(nickname, nicknameLength);
    AddChildTail(_nicknameLabel.GetPointer());
}

void RomBrowserTopScreenView::InitVram(const VramContext& vramContext)
{
    ViewContainer::InitVram(vramContext);
    auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        uint32_t offset = objVramManager->Alloc(status_bar::StatusGraphicsBytes);
        if (status_bar::StatusGraphicsBlockFits(offset))
        {
            _statusGraphicsOffset = offset;
            _statusGraphicsAddress = objVramManager->GetVramAddress(offset);
            _statusGraphicsReady = status_bar::StatusGraphicsWritable(offset, _statusGraphicsAddress != nullptr);
            _railGraphicsDirty = _statusGraphicsReady;
            _speakerGraphicsDirty = _statusGraphicsReady;
            _batteryGraphicsDirty = _statusGraphicsReady;
            _profileGraphicsDirty = _statusGraphicsReady;
        }
    }

    int tileIndex = 0;
    vu16* mapPtr = (vu16*)((u8*)GFX_BG_SUB + 0x3800);
    for (int y = 0; y < 12; y++)
    {
        for (int x = 0; x < 14; x++)
        {
            *mapPtr++ = tileIndex;
            tileIndex++;
        }
        mapPtr += 2;
    }
}

void RomBrowserTopScreenView::Update()
{
    if (_statusRefreshFrames == 0)
    {
        RefreshStatus();
        _statusRefreshFrames = 59;
    }
    else
    {
        _statusRefreshFrames--;
    }

    int selectedItem = _viewModel->GetSelectedItem();
    if (selectedItem != _lastSelectedItem && selectedItem >= 0)
    {
        auto& fileInfoManager = _viewModel->GetFileInfoManager();
        // GetInternalFileInfo() covers both game banners and custom icon overrides (the
        // latter apply to any file type, including folders), so check it directly instead
        // of branching on FileType::HasInternalFileInfo() - that's a static per-type property
        // and knows nothing about a per-item custom icon. IsFileInfoLoaded() distinguishes
        // "still loading" from "loaded, and there's legitimately nothing" so this waits for
        // the io thread instead of flashing the previous item's icon while undecided.
        if (fileInfoManager.IsFileInfoLoaded(selectedItem))
        {
            const auto& item = fileInfoManager.GetItem(selectedItem);
            auto info = fileInfoManager.GetInternalFileInfo(selectedItem);

            bool fileNameAsTitle = true;
            const char16_t* gameTitle = info ? info->GetGameTitle() : nullptr;
            if (gameTitle && gameTitle[0] != 0)
            {
                _fileInfoView->SetGameTitleAsync(_viewModel->GetBgTaskQueue(), gameTitle);
                fileNameAsTitle = false;
            }

            _selectedFileIcon = info ? info->CreateGameIcon() : nullptr;
            if (!_selectedFileIcon)
            {
                _selectedFileIcon = item.GetFileType()->CreateFileIcon("", _themeFileIconFactory);
            }
            if (_selectedFileIcon)
            {
                _selectedFileIcon->SetAnimFrame(_viewModel->GetIconFrameCounter());
                _iconGraphicsUploaded = false;
            }
            _fileInfoView->SetIcon(std::move(_selectedFileIcon));
            _fileInfoView->SetFileNameAsync(_viewModel->GetBgTaskQueue(), item.GetFileName(), fileNameAsTitle);

            _lastSelectedItem = selectedItem;

            auto cover = fileInfoManager.GetFileCover(selectedItem);
            if (cover.IsValid())
            {
                _selectedFileCover = std::move(cover);
                _coverGraphicsUploaded = false;
            }
        }
    }
    ViewContainer::Update();
}

void RomBrowserTopScreenView::RefreshStatus()
{
    SystemStatus status;
    rtc_readStatus(&status);
    status_bar::Snapshot snapshot
    {
        status.dateTime.date.month,
        status.dateTime.date.monthDay,
        status.dateTime.time.hour,
        status.dateTime.time.minute,
        status.batteryLevel,
        (status.rtcStatus & RTC_STATUS1_24HOUR) != 0,
        status.dateTime.time.amPm == RTC_PM,
        (status.flags & SYSTEM_STATUS_DSI) != 0,
        (status.flags & SYSTEM_STATUS_CHARGING) != 0
    };

    auto presentation = status_bar::MakePresentation(snapshot,
        (status.flags & SYSTEM_STATUS_VOLUME_VALID) != 0, status.volumeLevel);
    if (presentation.batteryFrame != _statusPresentation.batteryFrame)
        _batteryGraphicsDirty = _statusGraphicsReady;
    _statusPresentation = presentation;
    _statusLayout = status_bar::MakeLayout(snapshot.dsiMode);
    _centerStatusLabel->SetPosition((_statusGraphicsReady ? _statusLayout.dateTime : _statusLayout.fallbackDateTime).left, 0);
    _batteryStateLabel->SetPosition(
        (_statusGraphicsReady && presentation.ntrBattery ? _statusLayout.ntrText : _statusLayout.fallbackBatteryState).left, 0);

    auto centerText = status_bar::FormatCenter(snapshot);
    if (centerText != _centerStatusText)
    {
        _centerStatusText = centerText;
        _centerStatusLabel->SetText(_centerStatusText.data());
    }

    auto batteryStateText = status_bar::FormatBatteryFallback(presentation);
    if (batteryStateText != _batteryStateText)
    {
        _batteryStateText = batteryStateText;
        _batteryStateLabel->SetText(_batteryStateText.data());
    }
}

void RomBrowserTopScreenView::Draw(GraphicsContext& graphicsContext)
{
    _fileInfoView->Draw(graphicsContext);
    if (_statusGraphicsReady)
    {
        u32 paletteRow = graphicsContext.GetPaletteManager().AllocRow(
            DirectPalette(_statusPalette.data()), 0, 15);
        auto railOams = graphicsContext.GetOamManager().AllocOams(status_bar::RailOamEntries);
        for (uint8_t index = 0; index < status_bar::RailOamEntries; index++)
        {
            OamBuilder::OamWithSize<32, 16>(index * 32, 0,
                (_statusGraphicsOffset + status_bar::RailGraphicsOffset) >> 7)
                .WithPalette16(paletteRow)
                .WithPriority(graphicsContext.GetPriority())
                .Build(railOams[index]);
        }

        _centerStatusLabel->Draw(graphicsContext);
        if (_statusPresentation.ntrBattery)
            _batteryStateLabel->Draw(graphicsContext);
        _nicknameLabel->Draw(graphicsContext);

        if (_statusPresentation.volumeVisible)
        {
            auto speaker = graphicsContext.GetOamManager().AllocOams(1);
            OamBuilder::OamWithSize<16, 16>(_statusLayout.speaker.left, _statusLayout.speaker.top,
                (_statusGraphicsOffset + status_bar::SpeakerGraphicsOffset +
                    _statusPresentation.speakerFrame * status_bar::SpeakerFrameBytes) >> 7)
                .WithPalette16(paletteRow)
                .WithPriority(graphicsContext.GetPriority())
                .Build(speaker[0]);
        }
        auto battery = graphicsContext.GetOamManager().AllocOams(1);
        OamBuilder::OamWithSize<16, 8>(
            _statusLayout.battery.left, _statusLayout.battery.top,
            (_statusGraphicsOffset + status_bar::BatteryGraphicsOffset) >> 7)
            .WithPalette16(paletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(battery[0]);
        auto profile = graphicsContext.GetOamManager().AllocOams(1);
        OamBuilder::OamWithSize<16, 16>(_statusLayout.profileAvatar.left, _statusLayout.profileAvatar.top,
            (_statusGraphicsOffset + status_bar::ProfileGraphicsOffset) >> 7)
            .WithPalette16(paletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(profile[0]);
    }
    else
    {
        _centerStatusLabel->Draw(graphicsContext);
        _batteryStateLabel->Draw(graphicsContext);
        _nicknameLabel->Draw(graphicsContext);
    }
}

void RomBrowserTopScreenView::VBlank()
{
    ViewContainer::VBlank();

    if (_statusGraphicsReady)
    {
        auto graphics = (volatile uint8_t*)_statusGraphicsAddress;
        if (_railGraphicsDirty)
        {
            std::array<uint8_t, status_bar::RailGraphicsBytes> rail{};
            MakeRailGraphics(rail);
            memcpy((void*)(graphics + status_bar::RailGraphicsOffset), rail.data(), rail.size());
            _railGraphicsDirty = false;
        }
        if (_speakerGraphicsDirty)
        {
            memcpy((void*)(graphics + status_bar::SpeakerGraphicsOffset), statusSpeakerTiles,
                statusSpeakerTilesLen);
            _speakerGraphicsDirty = false;
        }
        if (_batteryGraphicsDirty)
        {
            const auto* batteryTiles = (const uint8_t*)statusBatteryTiles;
            memcpy((void*)(graphics + status_bar::BatteryGraphicsOffset),
                batteryTiles + _statusPresentation.batteryFrame * status_bar::BatteryFrameBytes,
                status_bar::BatteryFrameBytes);
            _batteryGraphicsDirty = false;
        }
        if (_profileGraphicsDirty)
        {
            const auto* profileTiles = (const uint8_t*)statusProfileTiles;
            memcpy((void*)(graphics + status_bar::ProfileGraphicsOffset),
                profileTiles, statusProfileTilesLen);
            _profileGraphicsDirty = false;
        }
    }

    if (!_coverGraphicsUploaded && _selectedFileCover.IsValid())
    {
        if (_showCover && _selectedFileCover->IsActualCover())
        {
            _selectedFileCover->Upload2DCoverBitmap((u8*)GFX_BG_SUB + 0x4000);
            mem_setVramHMapping(MEM_VRAM_H_LCDC);
            _selectedFileCover->Upload2DCoverPalette((void*)0x0689E000);
            GFX_PLTT_BG_SUB[0] = *(vu16*)0x0689E000;
            mem_setVramHMapping(MEM_VRAM_H_SUB_BG_EXT_PLTT_SLOT_0123);
        }
        _coverGraphicsUploaded = true;
    }
    int x0 = std::clamp(_coverPosition.x, 0, 256);
    int x1 = std::clamp(_coverPosition.x + 106, 0, 256);
    int y0 = std::clamp(_coverPosition.y, 0, 192);
    int y1 = std::clamp(_coverPosition.y + 96, 0, 192);
    if (!_showCover || !_selectedFileCover.IsValid() || !_selectedFileCover->IsActualCover() ||
        x0 >= x1 || y0 >= y1)
    {
        // hide cover
        REG_DISPCNT_SUB &= ~(((1 << 3) | (1 << 5)) << 8);
    }
    else
    {
        // display cover
        REG_BG3PA_SUB = 0x100;
        REG_BG3PB_SUB = 0;
        REG_BG3PC_SUB = 0;
        REG_BG3PD_SUB = -0x100;
        REG_BG3X_SUB = (-_coverPosition.x) << 8;
        REG_BG3Y_SUB = (96 + _coverPosition.y - 1) << 8;
        REG_BG3CNT_SUB = 0x0705;
        REG_DISPCNT_SUB |= ((1 << 3) | (1 << 5)) << 8;
        gfx_setSubWindow0(x0, y0, x1, y1);
        REG_WININ_SUB = 0x002A;
        REG_WINOUT_SUB = ~(1 << 3);
    }
    if (!_iconGraphicsUploaded)
    {
        _fileInfoView->UploadIconGraphics();
        _iconGraphicsUploaded = true;
    }
}
