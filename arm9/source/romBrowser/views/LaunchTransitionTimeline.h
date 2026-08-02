#pragma once
#include "themes/LaunchTransitionStyle.h"

struct LaunchTransitionTimeline
{
    static constexpr int TransformFrames = 28;
    static constexpr int ProgressMax = 1024;
    static constexpr int FadeStart = ProgressMax * 2 / 3;
    static constexpr int CoverStartHalfWidth = 53;
    static constexpr int CoverStartHalfHeight = 48;

    static constexpr int CoverLeft(int progress, int startScalePercent)
    {
        const int startHalfWidth = CoverStartHalfWidth * startScalePercent / 100;
        return (128 - startHalfWidth) * (ProgressMax - progress) / ProgressMax;
    }

    static constexpr int CoverTop(int progress, int startScalePercent)
    {
        const int startHalfHeight = CoverStartHalfHeight * startScalePercent / 100;
        return (96 - startHalfHeight) * (ProgressMax - progress) / ProgressMax;
    }

    static constexpr int CoverAlpha(int progress, int finalAlpha)
    {
        return progress <= FadeStart
            ? 31
            : 31 - (progress - FadeStart) * (31 - finalAlpha) / (ProgressMax - FadeStart);
    }

    static constexpr int ScrimAlpha(int progress, int finalAlpha)
    {
        return progress <= FadeStart
            ? 1
            : 1 + (progress - FadeStart) * (finalAlpha - 1) / (ProgressMax - FadeStart);
    }

    static constexpr int ProgressBarPhase(unsigned frame)
    {
        return (frame * 3) % 20;
    }
};

static_assert(LaunchTransitionTimeline::CoverLeft(0, LaunchTransitionStyle::DefaultCoverStartScalePercent) == 75);
static_assert(LaunchTransitionTimeline::CoverLeft(LaunchTransitionTimeline::ProgressMax,
    LaunchTransitionStyle::DefaultCoverStartScalePercent) == 0);
static_assert(LaunchTransitionTimeline::CoverTop(0, LaunchTransitionStyle::DefaultCoverStartScalePercent) == 48);
static_assert(LaunchTransitionTimeline::CoverTop(LaunchTransitionTimeline::ProgressMax,
    LaunchTransitionStyle::DefaultCoverStartScalePercent) == 0);
static_assert(LaunchTransitionTimeline::CoverAlpha(0, LaunchTransitionStyle::DefaultCoverFinalAlpha) == 31);
static_assert(LaunchTransitionTimeline::CoverAlpha(LaunchTransitionTimeline::ProgressMax,
    LaunchTransitionStyle::DefaultCoverFinalAlpha) == 12);
static_assert(LaunchTransitionTimeline::ScrimAlpha(0, LaunchTransitionStyle::DefaultScrimFinalAlpha) == 1);
static_assert(LaunchTransitionTimeline::ScrimAlpha(LaunchTransitionTimeline::ProgressMax,
    LaunchTransitionStyle::DefaultScrimFinalAlpha) == 14);
static_assert(LaunchTransitionTimeline::ProgressBarPhase(1) == 3);
