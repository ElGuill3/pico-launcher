#pragma once
#include <array>
#include "../background/IThemeBackground.h"
#include "../background/StatusBackgroundContracts.h"
#include "MaterialColorScheme.h"

class MaterialSubBackground : public IThemeBackground
{
public:
    explicit MaterialSubBackground(const MaterialColorScheme* materialColorScheme)
        : _materialColorScheme(materialColorScheme) { }

    void LoadResources(const ITheme& theme, const VramContext& vramContext) override;
    void VBlank() override;

    bool SupportsStatusComposition() const { return _statusSupported; }
    uint16_t StatusPaletteEndpoint() const { return _statusPalette[16]; }
    status_background::StatusPresentationMode GetStatusPresentationMode() const override
    {
        return status_background::MaterialCompositionMode(true, _statusSupported);
    }
    uint16_t GetStatusPaletteEndpoint() const override { return StatusPaletteEndpoint(); }

private:
    const MaterialColorScheme* _materialColorScheme;
    std::array<uint16_t, 32> _statusPalette{};
    bool _statusSupported = false;
};
