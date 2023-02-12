
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
        // delay(300);
    }
}

void sysMainInitial()
{
    UserCommonExtruderInitial();

    UserCommonWebServerSettingInitial();
}