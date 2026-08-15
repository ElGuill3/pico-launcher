#pragma once
#include <array>
#include "../background/IThemeBackground.h"
#include "../background/StatusBackgroundContracts.h"

class CustomSubBackground : public IThemeBackground
{
public:
    ~CustomSubBackground() override = default;
    void VBlank() override;
    void LoadResources(const ITheme& theme, const VramContext& vramContext) override;
    status_strip::StatusCompositionResult PrepareStatusStrip(
        const status_strip::StatusStripRequest& request) override;
    void RestoreStatusStrip() override;
    status_background::StatusPresentationMode GetStatusPresentationMode() const override
    {
        return status_background::CustomCompositionMode(true, _statusStripAvailable);
    }

private:
    std::array<uint8_t, status_background::StripBytes> _pristineStrip{};
    std::array<uint8_t, status_background::StripBytes> _composedStrip{};
    status_background::StatusStripUploadPlan _statusStripUpload{ 0, 0, 0, 0 };
    status_background::StatusStripUploadPlan _publishedStatusStripUpload{ 0, 0, 0, 0 };
    bool _statusStripAvailable = false;
    bool _statusStripDirty = false;
};
