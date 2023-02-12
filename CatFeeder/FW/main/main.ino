/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "src/system/sysMain.h"

void setup()
{
  Serial.begin(115200);

  delay(1000);
}

void loop()
{
  Serial.println("START..");
  sysMainProcess();
}
