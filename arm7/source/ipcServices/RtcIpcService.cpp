#include "common.h"
#include <nds/system.h>
#include <libtwl/i2c/i2cMcu.h>
#include <libtwl/sio/sioRtc.h>
#include "systemStatus.h"
#include "RtcIpcService.h"

void RtcIpcService::HandleMessage(u32 data)
{
    auto status = reinterpret_cast<SystemStatus*>(data << 2);
    rtc_readDateTime(&status->dateTime);
    status->rtcStatus = rtc_readStatus1();

    u32 battery = getBatteryLevel();
    status->batteryLevel = battery & 0x0F;
    status->volumeLevel = 0;
    status->flags = battery & 0x80 ? SYSTEM_STATUS_CHARGING : 0;
    if (isDSiMode())
    {
        status->flags |= SYSTEM_STATUS_DSI | SYSTEM_STATUS_VOLUME_VALID;
        status->volumeLevel = mcu_readReg(MCU_REG_VOLUME) & 0x1F;
    }
    SendResponseMessage(1);
}
