#pragma once
#include <memory>
#include "animation/Animator.h"
#include "gui/views/Label2DView.h"
#include "gui/VBlankTextureLoader.h"
#include "themes/material/MaterialColorScheme.h"
#include "../FileType/FileIcon.h"
#include "LaunchVisualSnapshot.h"

class IFontRepository;
class IThemeFileIconFactory;

class LaunchTransitionView : public View
{
    SHARED_ONLY(LaunchTransitionView)

public:
    ~LaunchTransitionView() override;

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    void VBlank() override;

    Rectangle GetBounds() const override { return Rectangle(0, 0, 256, 192); }
    bool IsMinimumComplete() const { return _transformAnimator.IsFinished(); }

private:
    LaunchVisualSnapshot _snapshot;
    const MaterialColorScheme* _materialColorScheme;
    const IThemeFileIconFactory* _themeFileIconFactory;
    VBlankTextureLoader* _vblankTextureLoader;
    SharedPtr<Label2DView> _titleLabel;
    std::unique_ptr<FileIcon> _fallbackIcon;
    Animator<int> _transformAnimator;
    VBlankTextureLoadRequest _coverLoadRequest;
    u32 _coverTexVramOffset = 0;
    u32 _coverPlttVramOffset = 0;
    u32 _iconVramOffset = 0;
    vu16* _iconVramAddress = nullptr;
    u32 _frame = 0;

    LaunchTransitionView(LaunchVisualSnapshot snapshot,
        const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository,
        const IThemeFileIconFactory* themeFileIconFactory,
        VBlankTextureLoader* vblankTextureLoader);

    void DrawCover(GraphicsContext& graphicsContext, int progress) const;
    void DrawScrimAndProgress(GraphicsContext& graphicsContext, int progress) const;
    void DrawIcon(GraphicsContext& graphicsContext);
};
