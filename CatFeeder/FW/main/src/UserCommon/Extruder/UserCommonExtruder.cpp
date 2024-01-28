#include <Arduino.h>
#include "UserCommonExtruder.h"
// #include <Stepper.h>
#include <AccelStepper.h>

// Stepper stepper(_STEP, _MOTOR_PIN_A, _MOTOR_PIN_AP, _MOTOR_PIN_B, _MOTOR_PIN_BP);
//  ULN2003 Motor Driver Pins

// const int stepsPerRevolution = 2048; // change this to fit the number of steps per revolution

int stepsPerRevolution = 64;     // steps per revolution
int degreePerRevolution = 5.625; // degree per revolution
#define motorPin1 5              // IN1 pin on the ULN2003A driver to pin D1 on NodeMCU board
#define motorPin2 4              // IN2 pin on the ULN2003A driver to pin D2 on NodeMCU board
#define motorPin3 0              // IN3 pin on the ULN2003A driver to pin D3 on NodeMCU board
#define motorPin4 2              // IN4 pin on the ULN2003A driver to pin D4 on NodeMCU board

AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
/*
 * Converts degrees to steps
 *
 * 28BYJ-48 motor has 5.625 degrees per step
 * 360 degrees / 5.625 = 64 steps per revolution
 *
 * Example with degToSteps(45):
 * (64 / 5.625) * 45 = 512 steps
 */

int targetSteps = 0;
int targetDegrees = 0;
float degToSteps(float deg)
{
    return (stepsPerRevolution / degreePerRevolution) * deg;
}

void UserCommonExtruderInitial()
{
    Serial.println("stepper initial...");
    stepper.setMaxSpeed(1000.0);    // set the max motor speed
    stepper.setAcceleration(100.0); // set the acceleration
    stepper.setSpeed(200);
    Serial.println("stepper initial done.");
}
void UserCommonExtruderRun()
{
    if (!stepper.isRunning() && stepper.distanceToGo() == 0)
    {
        targetSteps = degToSteps(targetDegrees);
        stepper.move(targetSteps);
        Serial.print("targetSteps: ");
        Serial.println(targetSteps);
    }
}

// Degrees of the momevement. So first to 90 degrees, then to -90 graden then to 180 degrees etc.
int steps[] = {90, -90, 180, -180, 0};

// The total entries in steps[]
int stepsCount = 5;

// Keeps track of the position in steps[] we are about to run
int stepsIndex = 0;
void UserCommonExtruderHandler()
{
    stepper.run();
}

void UserCommonExtruderSetValue(int value)
{
    Serial.print("value: ");
    Serial.println(value);
    targetDegrees = value;
}

void UserCommonManualFeedOutput(int amount)
{
    Serial.print("feed amount: ");
    Serial.println(amount);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    if (amount > 0)
    {
        pinMode(14, OUTPUT); // D5 blue
        digitalWrite(14, LOW);
        delay(3000);
        pinMode(14, OUTPUT); // D5
        digitalWrite(14, HIGH);
        for (int i = 0; i < amount; i++)
        {
            pinMode(12, OUTPUT); // D6 purple
            digitalWrite(12, LOW);
            delay(200);

            pinMode(12, OUTPUT); // D6
            digitalWrite(12, HIGH);

            delay(1500);
            Serial.print("feed done : ");
            Serial.println(i);
        }
    }
    digitalWrite(2, HIGH);
}
