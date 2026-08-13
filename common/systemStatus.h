#pragma once
#ifdef LIBTWL_ARM9
#include <../../libtwl7/include/libtwl/sio/sioRtc.h>
#else
#include <libtwl/sio/sioRtc.h>
#endif

enum SystemStatusFlags
{
    SYSTEM_STATUS_DSI = 1 << 0,
    SYSTEM_STATUS_CHARGING = 1 << 1,
    SYSTEM_STATUS_VOLUME_VALID = 1 << 2
};

struct SystemStatus
{
    rtc_datetime_t dateTime;
    u8 rtcStatus;
    u8 batteryLevel;
    u8 volumeLevel;
    u8 flags;
};
