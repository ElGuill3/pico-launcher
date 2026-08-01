#pragma once

enum class RomBrowserStateTrigger
{
    None,
    Navigate,
    ChangeDisplayMode,
    ShowGameInfo,
    HideGameInfo,
    FolderLoadDone,
    Launch,
    LaunchFailed,
    ShowDisplaySettings,
    HideDisplaySettings,
    GotoSettingsScreen
};
