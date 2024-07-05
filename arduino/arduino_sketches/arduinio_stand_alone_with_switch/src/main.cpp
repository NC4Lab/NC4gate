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
int pinSwitch = 2;  // Switch pin (white wire)

// PWM settings
uint8_t pwmUpFreq = 255;   // PWM dowm movement frequency [0, 255]
uint8_t pwmDownFreq = 255; // PWM up movement frequency [0, 255]

// Track toggle switch state
int switchStateLast = 2;
int switchState = 0;

//=============== FUNCTIONS =============
/**
 * @brief Check the limit switches to see if they are triggered.
 *
 * @return 1 if the down limit switch is triggered, 2 if the up limit switch is triggered, 0 otherwise.
 */
byte checkLimitSwitches()
{
  // Check the limit switches
  if (digitalRead(pinDownIO) == HIGH)
  {
    return 1;
  }
  if (digitalRead(pinUpIO) == HIGH)
  {
    return 2;
  }
  return 0;
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

  // Initialize the switch pin as an input with a pullup resistor
  pinMode(pinSwitch, INPUT_PULLUP);
}

//=============== LOOP ==================
void loop()
{
  // Get switch state
  switchState = digitalRead(pinSwitch);

  // Make sure the gate is stopped if the limit switch is pressed
  if (checkLimitSwitches() == 2)
  {
    analogWrite(pinUpPWM, 0);
  }
  else if (checkLimitSwitches() == 1)
  {
    analogWrite(pinDownPWM, 0);
  }

  // Check if the switch state has changed
  if (switchState == switchStateLast)
  {
    return;
  }

  // Update the switch state
  switchStateLast = switchState;

  // Check the toggle switch state and move the wall up or down
  if (switchState == LOW &&      // Switch is pressed
      checkLimitSwitches() != 2) // Wall is not at the top
  {
    // Move the gate up
    analogWrite(pinUpPWM, pwmUpFreq);
    Serial.println("Moving up");
  }
  else if (switchState == HIGH &&     // Switch is not pressed
           checkLimitSwitches() != 1) // Wall is not at the bottom
  {
    // Move the gate down
    analogWrite(pinDownPWM, pwmDownFreq);
    Serial.println("Moving down");
  }
}