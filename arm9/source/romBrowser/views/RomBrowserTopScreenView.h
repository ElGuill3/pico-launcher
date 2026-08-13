#pragma once
#include <array>
#include <cstdint>
#include "core/SharedPtr.h"
#include "gui/views/ViewContainer.h"
#include "gui/views/Label2DView.h"
#include "BannerView.h"
#include "StatusBarFormat.h"
#include "../FileType/FileIcon.h"
#include "../DisplayMode/RomBrowserDisplayMode.h"

class RomBrowserViewModel;
class IRomBrowserViewFactory;
class IFontRepository;
class MaterialColorScheme;

class RomBrowserTopScreenView : public ViewContainer
{
    SHARED_ONLY(RomBrowserTopScreenView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;

    Rectangle GetBounds() const override
    {
        return Rectangle(0, 0, 256, 192);
    }

private:
    SharedPtr<RomBrowserViewModel> _viewModel;
    const IThemeFileIconFactory* _themeFileIconFactory;
    SharedPtr<BannerView> _fileInfoView;
    SharedPtr<Label2DView> _centerStatusLabel;
    SharedPtr<Label2DView> _batteryStateLabel;
    SharedPtr<Label2DView> _nicknameLabel;
    std::unique_ptr<FileIcon> _selectedFileIcon;
    SharedPtr<FileCover> _selectedFileCover;
    status_bar::CenterText _centerStatusText{};
    status_bar::BatteryText _batteryStateText{};
    status_bar::PresentationState _statusPresentation{};
    status_bar::Layout _statusLayout{};
    std::array<uint16_t, 16> _statusPalette{};
    uint32_t _statusGraphicsOffset = 0;
    volatile uint16_t* _statusGraphicsAddress = nullptr;
    int _lastSelectedItem = -1;
    int _statusRefreshFrames = 0;
    bool _iconGraphicsUploaded = false;
    bool _coverGraphicsUploaded = false;
    bool _statusGraphicsReady = false;
    bool _railGraphicsDirty = false;
    bool _speakerGraphicsDirty = false;
    bool _batteryGraphicsDirty = false;
    bool _profileGraphicsDirty = false;
    bool _showCover;
    Point _coverPosition;

    RomBrowserTopScreenView(SharedPtr<RomBrowserViewModel> viewModel,
        const RomBrowserDisplayMode* displayMode,
        const IThemeFileIconFactory* themeFileIconFactory,
        const IRomBrowserViewFactory* romBrowserViewFactory,
        const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository);

    void RefreshStatus();
};
