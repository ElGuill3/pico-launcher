#pragma once

class LaunchTransitionStyle
{
public:
    static constexpr int DefaultCoverStartScalePercent = 100;
    static constexpr int DefaultCoverFinalAlpha = 12;
    static constexpr int DefaultScrimFinalAlpha = 14;

    static constexpr int MinCoverStartScalePercent = 1;
    static constexpr int MaxCoverStartScalePercent = 200;
    static constexpr int MinAlpha = 0;
    static constexpr int MaxAlpha = 31;

    constexpr LaunchTransitionStyle(
        int coverStartScalePercent = DefaultCoverStartScalePercent,
        int coverFinalAlpha = DefaultCoverFinalAlpha,
        int scrimFinalAlpha = DefaultScrimFinalAlpha)
        : _coverStartScalePercent(coverStartScalePercent)
        , _coverFinalAlpha(coverFinalAlpha)
        , _scrimFinalAlpha(scrimFinalAlpha) { }

    constexpr int GetCoverStartScalePercent() const { return _coverStartScalePercent; }
    constexpr int GetCoverFinalAlpha() const { return _coverFinalAlpha; }
    constexpr int GetScrimFinalAlpha() const { return _scrimFinalAlpha; }

private:
    int _coverStartScalePercent;
    int _coverFinalAlpha;
    int _scrimFinalAlpha;
};

static_assert(LaunchTransitionStyle::DefaultCoverStartScalePercent >= LaunchTransitionStyle::MinCoverStartScalePercent);
static_assert(LaunchTransitionStyle::DefaultCoverStartScalePercent <= LaunchTransitionStyle::MaxCoverStartScalePercent);
static_assert(LaunchTransitionStyle::DefaultCoverFinalAlpha >= LaunchTransitionStyle::MinAlpha);
static_assert(LaunchTransitionStyle::DefaultCoverFinalAlpha <= LaunchTransitionStyle::MaxAlpha);
static_assert(LaunchTransitionStyle::DefaultScrimFinalAlpha >= LaunchTransitionStyle::MinAlpha);
static_assert(LaunchTransitionStyle::DefaultScrimFinalAlpha <= LaunchTransitionStyle::MaxAlpha);
static_assert(LaunchTransitionStyle::MaxAlpha == 31);
static_assert(LaunchTransitionStyle().GetCoverStartScalePercent() ==
    LaunchTransitionStyle::DefaultCoverStartScalePercent);
static_assert(LaunchTransitionStyle().GetCoverFinalAlpha() ==
    LaunchTransitionStyle::DefaultCoverFinalAlpha);
static_assert(LaunchTransitionStyle().GetScrimFinalAlpha() ==
    LaunchTransitionStyle::DefaultScrimFinalAlpha);
