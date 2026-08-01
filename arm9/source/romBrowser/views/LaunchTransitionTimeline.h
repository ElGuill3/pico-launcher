#pragma once

struct LaunchTransitionTimeline
{
    static constexpr int TransformFrames = 28;
    static constexpr int ProgressMax = 1024;
    static constexpr int FadeStart = ProgressMax * 2 / 3;
    static constexpr int CoverFinalAlpha = 12;
    static constexpr int ScrimFinalAlpha = 14;

    static constexpr int CoverAlpha(int progress)
    {
        return progress <= FadeStart
            ? 31
            : 31 - (progress - FadeStart) * (31 - CoverFinalAlpha) / (ProgressMax - FadeStart);
    }

    static constexpr int ScrimAlpha(int progress)
    {
        return progress <= FadeStart
            ? 1
            : 1 + (progress - FadeStart) * (ScrimFinalAlpha - 1) / (ProgressMax - FadeStart);
    }

    static constexpr int ProgressBarPhase(unsigned frame)
    {
        return (frame * 3) % 20;
    }
};

static_assert(LaunchTransitionTimeline::CoverAlpha(0) == 31);
static_assert(LaunchTransitionTimeline::CoverAlpha(LaunchTransitionTimeline::ProgressMax) == 12);
static_assert(LaunchTransitionTimeline::ScrimAlpha(0) == 1);
static_assert(LaunchTransitionTimeline::ScrimAlpha(LaunchTransitionTimeline::ProgressMax) == 14);
static_assert(LaunchTransitionTimeline::ProgressBarPhase(1) == 3);
