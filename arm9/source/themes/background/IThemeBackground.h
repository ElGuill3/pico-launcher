#pragma once

#include "StatusStripComposition.h"
#include "StatusBackgroundContracts.h"

class GraphicsContext;
class VramContext;
class IVramManager;
class ITheme;

class IThemeBackground
{
public:
    virtual ~IThemeBackground() = 0;

    /// @brief Updates the background.
    virtual void Update() { }

    /// @brief Draws the background.
    /// @param graphicsContext The graphics context to use.
    virtual void Draw(GraphicsContext& graphicsContext) { }

    /// @brief Performs vblank processes for the background.
    virtual void VBlank() { }

    /// @brief Synchronously prepares a status strip. Input spans are consumed before return.
    virtual status_strip::StatusCompositionResult PrepareStatusStrip(
        const status_strip::StatusStripRequest& request)
    {
        return { status_strip::CompositionState::Invalid, false };
    }

    /// @brief Restores any composed status strip before a lifecycle transition.
    virtual void RestoreStatusStrip() { }

    virtual status_background::StatusPresentationMode GetStatusPresentationMode() const
    {
        return status_background::StatusPresentationMode::Unsupported;
    }

    virtual uint16_t GetStatusPaletteEndpoint() const { return 0; }

    /// @brief Loads the resources for this background.
    /// @param theme The theme the background belongs to.
    /// @param vramContext The vram context.
    virtual void LoadResources(const ITheme& theme, const VramContext& vramContext) { }
};

inline IThemeBackground::~IThemeBackground() { }
