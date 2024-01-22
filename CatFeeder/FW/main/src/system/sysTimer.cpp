#ifndef SYSMAIN_H
#define SYSMAIN_H
#include "sysTimer.h"
#include "../../src/UserCommon/Web/UserCommonWebControl.h"

#include <Arduino.h>

bool bSystemTimerEvent_KeyBoardState = false;
bool bSystemTimerEvent_PowerDetect = false;
bool bSystemTimerEvent_systemTimer = false;
bool bSystemTimerEvent_checkFeedSchedule = false;
unsigned long lastTimerValue = 0;

/// sytermTimerEventHander : check current events status
void sysTimerEventHander()
{
    unsigned long newTimeValue = millis();
    unsigned long timerValue = 0;
    if (lastTimerValue > newTimeValue)
    {
        timerValue = (4294967295 - lastTimerValue) + newTimeValue;
    }
    else
    {
        timerValue = newTimeValue - lastTimerValue;
    }
    if (bSystemTimerEvent_checkFeedSchedule)
    {
        if (timerValue >= 60000)
        {
            UserCommonCheckFeedSchedule();
            lastTimerValue = newTimeValue;
        }
    }

    delay(100);
}

void sysTimerEventActive(EnumSystemTimerEventId enumSystemTimerEventId)
{
    switch (enumSystemTimerEventId)
    {
    case _SYSTEM_CHECK_SCHEDULE:
        bSystemTimerEvent_checkFeedSchedule = true;
        break;
    default:
        break;
    }
}

void sysTimerEventClear(EnumSystemTimerEventId enumSystemTimerEventId)
{
}
#endif