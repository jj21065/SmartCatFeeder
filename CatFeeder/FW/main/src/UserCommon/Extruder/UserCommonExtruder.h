#ifndef USERCOMMONEXTRUDER_H
#define USERCOMMONEXTRUDER_H
#include "../../../PinshareSetting.h"
void UserCommonExtruderInitial();
void UserCommonExtruderRun();
void UserCommonExtruderSetValue(int value);
void UserCommonExtruderHandler();
void UserCommonTestDigitalOutput(bool enable);
#endif