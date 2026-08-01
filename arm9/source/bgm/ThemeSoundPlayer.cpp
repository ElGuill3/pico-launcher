#include "common.h"
#include <memory>
#include <string.h>
#include <nds/arm9/cache.h>
#include <libtwl/ipc/ipcFifoSystem.h>
#include <../../libtwl7/include/libtwl/sound/soundChannel.h>
#include "ipcChannels.h"
#include "themes/ITheme.h"
#include "ThemeSoundPlayer.h"

namespace
{
u16 ReadLe16(const u8* value)
{
    return value[0] | (value[1] << 8);
}

u32 ReadLe32(const u8* value)
{
    return static_cast<u32>(value[0]) |
        (static_cast<u32>(value[1]) << 8) |
        (static_cast<u32>(value[2]) << 16) |
        (static_cast<u32>(value[3]) << 24);
}

bool IsChunkId(const u8* value, const char* id)
{
    return memcmp(value, id, 4) == 0;
}
}

ThemeSoundPlayer::ThemeSoundPlayer()
    : _soundPlayCmdList
    {
        3,
        (ChannelMask << 8) | SND_IPC_CMD_STOP_CHANNELS,
        {
            SND_IPC_CMD_SETUP_CHANNEL,
            Channel,
            _samples,
            0,
            0,
            0,
            SOUNDCNT_VOLUME(80) | SOUNDCNT_PAN(64) | SOUNDCNT_MODE_ONCE | SOUNDCNT_FORMAT_PCM16
        },
        (ChannelMask << 8) | SND_IPC_CMD_START_CHANNELS
    }
    , _soundStopCmdList
    {
        1,
        (ChannelMask << 8) | SND_IPC_CMD_STOP_CHANNELS
    }
{
    DC_FlushRange(&_soundStopCmdList, sizeof(_soundStopCmdList));
}

ThemeSoundPlayer::~ThemeSoundPlayer()
{
    Stop();
}

bool ThemeSoundPlayer::Load(const ITheme& theme, const TCHAR* themeRelativePath)
{
    Stop();
    _loaded = false;
    _playbackBytes = 0;

    File file;
    if (!theme.OpenThemeFile(file, themeRelativePath))
        return false;

    const u64 fileSize = file.GetSize();
    if (fileSize < 12 || fileSize > MaxFileBytes)
        return false;

    std::unique_ptr<u8[]> fileData(new(cache_align) u8[fileSize]);
    u32 bytesRead = 0;
    if (file.Read(fileData.get(), fileSize, bytesRead) != FR_OK || bytesRead != fileSize)
        return false;

    const u8* riffHeader = fileData.get();
    if (!IsChunkId(riffHeader, "RIFF") || !IsChunkId(riffHeader + 8, "WAVE"))
    {
        return false;
    }

    const u64 riffEnd = static_cast<u64>(ReadLe32(riffHeader + 4)) + 8;
    if (riffEnd < 12 || riffEnd > fileSize)
        return false;

    bool foundFormat = false;
    bool foundData = false;
    u32 dataOffset = 0;
    u32 dataSize = 0;
    u64 position = 12;
    while (position + 8 <= riffEnd)
    {
        const u8* chunkHeader = fileData.get() + position;
        const u32 chunkSize = ReadLe32(chunkHeader + 4);
        const u64 chunkDataOffset = position + 8;
        const u64 chunkEnd = chunkDataOffset + chunkSize;
        const u64 paddedChunkEnd = chunkEnd + (chunkSize & 1);
        if (chunkEnd > riffEnd || paddedChunkEnd > riffEnd)
            return false;

        if (IsChunkId(chunkHeader, "fmt ") && !foundFormat)
        {
            if (chunkSize < 16)
                return false;

            const u8* format = fileData.get() + chunkDataOffset;
            if (ReadLe16(format) != 1 ||
                ReadLe16(format + 2) != 1 ||
                ReadLe32(format + 4) != SampleRate ||
                ReadLe32(format + 8) != SampleRate * sizeof(s16) ||
                ReadLe16(format + 12) != sizeof(s16) ||
                ReadLe16(format + 14) != 16)
            {
                return false;
            }
            foundFormat = true;
        }
        else if (IsChunkId(chunkHeader, "data") && !foundData)
        {
            if (chunkSize == 0 || chunkSize > MaxPcmBytes || (chunkSize % sizeof(s16)) != 0)
                return false;

            dataOffset = chunkDataOffset;
            dataSize = chunkSize;
            foundData = true;
        }

        position = paddedChunkEnd;
    }

    if (position != riffEnd || !foundFormat || !foundData)
        return false;

    _playbackBytes = (dataSize + 15) & ~15u;
    memset(_samples, 0, _playbackBytes);
    memcpy(_samples, fileData.get() + dataOffset, dataSize);

    _soundPlayCmdList.setup.timer = -((33513982 + SampleRate) / (SampleRate * 2));
    _soundPlayCmdList.setup.length = _playbackBytes / sizeof(u32);
    DC_FlushRange(_samples, _playbackBytes);
    DC_FlushRange(&_soundPlayCmdList, sizeof(_soundPlayCmdList));
    _loaded = true;
    return true;
}

void ThemeSoundPlayer::Play()
{
    if (!_loaded)
        return;

    DC_FlushRange(_samples, _playbackBytes);
    ipc_sendFifoMessage(IPC_CHANNEL_SOUND, reinterpret_cast<u32>(&_soundPlayCmdList));
}

void ThemeSoundPlayer::Stop()
{
    if (_loaded)
        ipc_sendFifoMessage(IPC_CHANNEL_SOUND, reinterpret_cast<u32>(&_soundStopCmdList));
}
