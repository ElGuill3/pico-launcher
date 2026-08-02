#pragma once
#include "common.h"
#include "ThemeType.h"
#include "LaunchTransitionStyle.h"
#include "core/String.h"
#include "core/math/Rgb.h"

class ThemeInfo
{
    String<TCHAR, 64> _folderName;
    ThemeType _type;
    String<char16_t, 64> _name;
    String<char16_t, 128> _description;
    String<char16_t, 64> _author;
    Rgb<8, 8, 8> _primaryColor;
    bool _darkTheme;
    LaunchTransitionStyle _launchTransitionStyle;
public:
    ThemeInfo(const TCHAR* folderName, ThemeType type, const char* name,
        const char* description, const char* author, const Rgb<8, 8, 8>& primaryColor,
        bool darkTheme, const LaunchTransitionStyle& launchTransitionStyle = LaunchTransitionStyle())
        : _folderName(folderName), _type(type), _name(name)
        , _description(description), _author(author), _primaryColor(primaryColor)
        , _darkTheme(darkTheme), _launchTransitionStyle(launchTransitionStyle) { }

    ThemeInfo(const TCHAR* folderName, ThemeType type, const char16_t* name,
        const char16_t* description, const char16_t* author, const Rgb<8, 8, 8>& primaryColor,
        bool darkTheme, const LaunchTransitionStyle& launchTransitionStyle = LaunchTransitionStyle())
        : _folderName(folderName), _type(type), _name(name)
        , _description(description), _author(author), _primaryColor(primaryColor)
        , _darkTheme(darkTheme), _launchTransitionStyle(launchTransitionStyle) { }

    constexpr const TCHAR* GetFolderName() const { return _folderName; }
    constexpr ThemeType GetType() const { return _type; }
    constexpr const char16_t* GetName() const { return _name; }
    constexpr const char16_t* GetDescription() const { return _description; }
    constexpr const char16_t* GetAuthor() const { return _author; }
    constexpr const Rgb<8, 8, 8>& GetPrimaryColor() const { return _primaryColor; }
    constexpr bool GetIsDarkTheme() const { return _darkTheme; }
    constexpr const LaunchTransitionStyle& GetLaunchTransitionStyle() const { return _launchTransitionStyle; }
};
