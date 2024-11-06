// ######################################

// ============== main.cpp ==============

// ######################################

/**
 * @file Main Arduino file for running the maze.
 */

// BUILT IN
#include "Arduino.h"
#include <Wire.h>

//============ VARIABLES ===============

// Pin mapping
int pinDownIO = 7;  // Down limit switch pin (green wire)
int pinUpIO = 6;    // Up limit switch pin (yellow wire)
int pinUpPWM = 5;   // Up PWM pin (blue wire)
int pinDownPWM = 4; // Down PWM pin (brown/white wire)

// PWM settings
uint8_t pwmUpFreq = 255;   // PWM dowm movement frequency [0, 255]
uint8_t pwmDownFreq = 255; // PWM up movement frequency [0, 255]

//=============== FUNCTIONS =============

/**
 * @brief Check the limit switches to see if they are triggered.
 *
 * @return 1 if the down limit switch is triggered, 2 if the up limit switch is triggered, 0 otherwise.
 */
byte checkLimitSwitches()
{
  static byte switch_state = 255; // [1:down, 2:up]

  // Check the limit switches
  if (digitalRead(pinDownIO) == HIGH && switch_state != 1)
  {
    // Serial.println("Down limit switch triggered");
    switch_state = 1;
  }
  else if (digitalRead(pinUpIO) == HIGH && switch_state != 2)
  {
    // Serial.println("Up limit switch triggered");
    switch_state = 2;
  }
  else
  {
    return 0;
  }
  return switch_state;
}

//=============== SETUP =================
void setup()
{
  // Setup serial coms for serial monoitor
  /// @note Comment this out if SerialCom is using the "Serial" HardwareSerial port
  Serial.begin(115200);
  delay(100);

  // Initialize the limit switches
  pinMode(pinDownIO, INPUT);
  pinMode(pinUpIO, INPUT);

  // Initialize the PWM pins
  pinMode(pinUpPWM, OUTPUT);
  pinMode(pinDownPWM, OUTPUT);
}

//=============== LOOP ==================
void loop()
{
  // Move the gate up
  analogWrite(pinUpPWM, pwmUpFreq);

  // Check for the up limit switch
  while (checkLimitSwitches() != 2)
  {
    delay(10);
  }

  // Stop the gate
  analogWrite(pinUpPWM, 0);

  // Move the gate down
  analogWrite(pinDownPWM, pwmDownFreq);

  // Check for the down limit switch
  while (checkLimitSwitches() != 1)
  {
    delay(10);
  }

  // Stop the gate
  analogWrite(pinDownPWM, 0);
}