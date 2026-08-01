#pragma once
#include "common.h"
#include "soundIpcCommand.h"

class ITheme;

class alignas(32) ThemeSoundPlayer
{
public:
    ThemeSoundPlayer();
    ~ThemeSoundPlayer();

    bool Load(const ITheme& theme, const TCHAR* themeRelativePath);
    void Play();
    void Stop();

private:
    static constexpr u32 Channel = 2;
    static constexpr u32 ChannelMask = 1u << Channel;
    static constexpr u32 SampleRate = 22050;
    static constexpr u32 MaxFileBytes = 16 * 1024;
    static constexpr u32 MaxPcmBytes = 11024;

    struct alignas(32) SoundPlayCmdList
    {
        u32 cmdCount;
        u32 stopChannels;
        snd_ipc_cmd_setup_channel_t setup;
        u32 startChannels;
    };

    struct alignas(32) SoundStopCmdList
    {
        u32 cmdCount;
        u32 stopChannels;
    };

    u8 _samples[MaxPcmBytes] alignas(32);
    SoundPlayCmdList _soundPlayCmdList alignas(32);
    SoundStopCmdList _soundStopCmdList alignas(32);
    u32 _playbackBytes = 0;
    bool _loaded = false;
};
