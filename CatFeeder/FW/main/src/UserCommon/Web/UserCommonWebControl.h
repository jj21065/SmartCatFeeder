#ifndef USERCOMMONWEBCONTROL_H
#define USERCOMMONWEBCONTROL_H
#include "../../../PinshareSetting.h"

void UserCommonWebServerSettingInitial();

void UserCommonWebServerHandler();

void UserCommonWebGetGoogleSheet(String dataString);

void UserCommonWebPostGoogleSheet();

// void UserCommonWebCurrentTime()

void GetFeedSchedule();

void UpdateFeedSchedule();

#endif