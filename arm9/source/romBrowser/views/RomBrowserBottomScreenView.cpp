#include "common.h"
#include "../viewModels/RomBrowserViewModel.h"
#include "../views/IconGridItemView.h"
#include "gui/GraphicsContext.h"
#include "backIcon.h"
#include "settingsIcon.h"
#include "heartIcon.h"
#include "recentIcon.h"
#include "listIcon.h"
#include "gui/IVramManager.h"
#include "gui/input/InputProvider.h"
#include "core/StringUtil.h"
#include "LaunchTransitionView.h"
#include "RomBrowserBottomScreenView.h"

RomBrowserBottomScreenView::RomBrowserBottomScreenView(
    RomBrowserBottomScreenViewModel* viewModel,
    const RomBrowserDisplayMode* displayMode,
    const IThemeFileIconFactory* themeFileIconFactory,
    const IRomBrowserViewFactory* romBrowserViewFactory,
    VBlankTextureLoader* vblankTextureLoader)
    : _viewModel(viewModel)
    , _romBrowserViewFactory(romBrowserViewFactory)
    , _romBrowserDisplayMode(displayMode)
    , _themeFileIconFactory(themeFileIconFactory)
    , _romBrowserAppBarView(RomBrowserAppBarView::CreateShared(_viewModel->GetRomBrowserAppBarViewModel(),
        *displayMode, romBrowserViewFactory))
    , _vblankTextureLoader(vblankTextureLoader)
{
    _romBrowserAppBarView->SetParent(this);
}

void RomBrowserBottomScreenView::InitVram(const VramContext& vramContext)
{
    _romBrowserAppBarView->InitVram(vramContext);
}

void RomBrowserBottomScreenView::Update()
{
    if (_launchTransitionView)
    {
        _launchTransitionView->Update();
        return;
    }
    _romBrowserAppBarView->Update();
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->Update();
    }
}

void RomBrowserBottomScreenView::Draw(GraphicsContext& graphicsContext)
{
    if (_launchTransitionView)
    {
        _launchTransitionView->Draw(graphicsContext);
        return;
    }
    _romBrowserAppBarView->Draw(graphicsContext);
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->Draw(graphicsContext);
    }
}

void RomBrowserBottomScreenView::VBlank()
{
    if (_launchTransitionView)
    {
        _launchTransitionView->VBlank();
        return;
    }
    _romBrowserAppBarView->VBlank();
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->VBlank();
    }
}

LaunchVisualSnapshot RomBrowserBottomScreenView::CaptureLaunchVisualSnapshot() const
{
    LaunchVisualSnapshot snapshot;
    const auto& romBrowserViewModel = _viewModel->GetRomBrowserViewModel();
    if (!romBrowserViewModel)
        return snapshot;

    const int selectedItem = romBrowserViewModel->GetSelectedItem();
    auto& fileInfoManager = romBrowserViewModel->GetFileInfoManager();
    if (selectedItem < 0 || static_cast<u32>(selectedItem) >= fileInfoManager.GetItemCount())
        return snapshot;

    const auto& item = fileInfoManager.GetItem(selectedItem);
    StringUtil::Copy(snapshot.fileName, item.GetFileName(), LaunchVisualSnapshot::FileNameLength);
    snapshot.isNds = strcmp(item.GetFileType()->GetShortName(), "nds") == 0;
    snapshot.cover = fileInfoManager.GetFileCover(selectedItem);

    const auto info = fileInfoManager.GetInternalFileInfo(selectedItem);
    const char16_t* title = info ? info->GetGameTitle() : nullptr;
    if (title && title[0] != 0)
        StringUtil::Copy(snapshot.title, title, LaunchVisualSnapshot::TitleLength);
    else
        StringUtil::Copy(snapshot.title, snapshot.fileName, LaunchVisualSnapshot::TitleLength);

    snapshot.hasIcon = info && info->CopyGameIconData(snapshot.iconGraphics, snapshot.iconPalette);
    if (snapshot.hasIcon && snapshot.isNds)
    {
        snapshot.ndsBanner = std::make_unique<nds_banner_t>();
        if (!info->CopyGameBanner(snapshot.ndsBanner.get()))
            snapshot.ndsBanner.reset();
    }
    return snapshot;
}

void RomBrowserBottomScreenView::StartLaunchTransition(LaunchVisualSnapshot snapshot,
    const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository,
    const VramContext& vramContext)
{
    _launchTransitionView = LaunchTransitionView::CreateShared(std::move(snapshot),
        materialColorScheme, fontRepository, _themeFileIconFactory, _vblankTextureLoader);
    _launchTransitionView->SetParent(this);
    _launchTransitionView->InitVram(vramContext);
}

void RomBrowserBottomScreenView::ReleaseBrowserViewForLaunch()
{
    _romBrowserView.Reset();
}

void RomBrowserBottomScreenView::EndLaunchTransition()
{
    _launchTransitionView.Reset();
}

bool RomBrowserBottomScreenView::IsLaunchTransitionComplete() const
{
    return _launchTransitionView && _launchTransitionView->IsMinimumComplete();
}

SharedPtr<View> RomBrowserBottomScreenView::MoveFocus(const SharedPtr<View>& currentFocus, FocusMoveDirection direction, View* source)
{
    if (!currentFocus)
    {
        return nullptr;
    }
    if (source == _romBrowserAppBarView.GetPointer())
    {
        if (_romBrowserDisplayMode->IsVertical())
        {
            if (direction == FocusMoveDirection::Right)
            {
                return _romBrowserView->MoveFocus(currentFocus, direction, this);
            }
        }
        else
        {
            if (direction == FocusMoveDirection::Down)
            {
                return _romBrowserView->MoveFocus(currentFocus, direction, this);
            }
        }
        return nullptr;
    }
    else if (source == _romBrowserView.GetPointer())
    {
        if (_romBrowserDisplayMode->IsVertical())
        {
            if (direction == FocusMoveDirection::Left)
            {
                return _romBrowserAppBarView->MoveFocus(currentFocus, direction, this);
            }
        }
        else
        {
            if (direction == FocusMoveDirection::Up)
            {
                return _romBrowserAppBarView->MoveFocus(currentFocus, direction, this);
            }
        }
        return nullptr;
    }
    return nullptr;
}

bool RomBrowserBottomScreenView::HandleInput(const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->NavigateUp();
        return true;
    }
    return View::HandleInput(inputProvider, focusManager);
}

void RomBrowserBottomScreenView::HandlePenDown(const Point& touchPoint, FocusManager& focusManager)
{
    _romBrowserAppBarView->HandlePenDown(touchPoint, focusManager);
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->HandlePenDown(touchPoint, focusManager);
    }
}

void RomBrowserBottomScreenView::HandlePenMove(const Point& touchPoint, FocusManager& focusManager)
{
    _romBrowserAppBarView->HandlePenMove(touchPoint, focusManager);
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->HandlePenMove(touchPoint, focusManager);
    }
}

void RomBrowserBottomScreenView::HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager)
{
    _romBrowserAppBarView->HandlePenUp(lastTouchPoint, focusManager);
    if (_romBrowserView && _viewModel->IsRomBrowserVisible())
    {
        _romBrowserView->HandlePenUp(lastTouchPoint, focusManager);
    }
}

void RomBrowserBottomScreenView::RomBrowserViewModelInvalidated(const VramContext& vramContext)
{
    if (_viewModel->GetRomBrowserViewModel().IsValid())
    {
        _romBrowserView = RomBrowserView::CreateShared(
            _viewModel->GetRomBrowserViewModel(), *_romBrowserDisplayMode,
            _themeFileIconFactory, _romBrowserViewFactory, _vblankTextureLoader);
        _romBrowserView->SetParent(this);
        _romBrowserView->InitVram(vramContext);
    }
    else
    {
        _romBrowserView.Reset();
    }
}
