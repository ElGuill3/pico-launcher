#pragma once
#include "core/SharedPtr.h"
#include "../FileType/FileCover.h"

struct LaunchVisualSnapshot
{
    static constexpr u32 IconGraphicsSize = 512;
    static constexpr u32 IconPaletteSize = 16;
    static constexpr u32 TitleLength = 128;
    static constexpr u32 FileNameLength = 256;

    SharedPtr<FileCover> cover;
    alignas(32) u8 iconGraphics[IconGraphicsSize] = {};
    alignas(32) u16 iconPalette[IconPaletteSize] = {};
    char16_t title[TitleLength] = {};
    char fileName[FileNameLength] = {};
    bool hasIcon = false;
    bool isNds = false;
};
