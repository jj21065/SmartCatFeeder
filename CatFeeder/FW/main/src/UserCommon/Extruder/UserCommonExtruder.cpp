#include "UserCommonExtruder.h"
#include <Stepper.h>
#define _STEP 2048
Stepper stepper(_STEP, _MOTOR_PIN_A, _MOTOR_PIN_AP, _MOTOR_PIN_B, _MOTOR_PIN_BP);

void UserCommonExtruderInitial()
{
    stepper.setSpeed(12); // 設定轉速為1分鐘12圈，容許設定的範圍為每分鐘 0-15 圈
}
void UserCommonExtruderPush(int count)
{
    stepper.step(count);
}

void UserCommonExtruderExtract(int count)
{
    stepper.step(-count);
}