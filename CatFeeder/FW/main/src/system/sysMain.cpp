
#include <Arduino.h>
#include "sysMain.h"
#include "../../src/UserCommon/Web/UserCommonWebControl.h"
#include "../../src/UserCommon/Extruder/UserCommonExtruder.h"

void sysMainProcess()
{
    sysMainInitial();

    while (1)
    {

        UserCommonWebServerHandler();

        UserCommonExtruderHandler();

        delay(1000);
    }
}

void sysMainInitial()
{

    pinMode(14, OUTPUT); // D5
    digitalWrite(14, HIGH);

    pinMode(12, OUTPUT); // D6
    digitalWrite(12, HIGH);

    UserCommonExtruderInitial();

    UserCommonWebServerSettingInitial();

    // UserCommonTimerActive();
}