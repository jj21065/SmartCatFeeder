
#include <Arduino.h>
#include "sysMain.h"
#include "../../src/UserCommon/Web/UserCommonWebControl.h"

void sysMainProcess()
{
    Serial.println("TEST0..");
    sysMainInitial();

    while (1)
    {
        Serial.println("TEST..");
        UserCommonWebServerHandler();
        delay(1000);
    }
}

void sysMainInitial()
{
    UserCommonWebServerSettingInitial();
}