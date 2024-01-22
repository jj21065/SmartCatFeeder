#ifndef USERCOMMONWEBCONTROL_H
#define USERCOMMONWEBCONTROL_H
#include <vector>
#include <Arduino.h>
#include "../../../PinshareSetting.h"

void UserCommonWebServerSettingInitial();

void UserCommonWebServerHandler();

void UserCommonWebGetGoogleSheet(String dataString);

void UserCommonWebPostGoogleSheet();

void UserCommonWebCurrentTime(String &hrString, String &minString, String &secString);

void GetFeedSchedule();

void UpdateFeedSchedule();

void UserCommonCheckFeedSchedule();

class UserCommonScheduleStruct
{
    /* data */
public:
    String name;
    String date;
    String time;
    int amount;
    bool enable;
    bool isDone;
};

#endif