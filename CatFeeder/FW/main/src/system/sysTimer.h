// Because of the use of single thread mcu, we could use timer to wait event and not holding the thread in certain processes
//
#include "../UserCommon/UserCommonInclude.h"
typedef enum
{
    _SYSTEM_TIMER_EVENT_KEYBOARD_STATE,
    _SYSTEM_TIMER_EVENT_POWER_DETECTING,
    _SYSTEM_TEIMER,
    _SYSTEM_CHECK_SCHEDULE
} EnumSystemTimerEventId;

void sysTimerEventHander();

void sysTimerEventActive(EnumSystemTimerEventId enumSystemTimerEventId);

void sysTimerEventClear(EnumSystemTimerEventId enumSystemTimerEventId);