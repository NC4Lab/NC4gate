// ######################################

// ============== main.cpp ==============

// ######################################

/**
 * @file Main Arduino file for running the NC4gate.
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

// Track switch state and the number of up/down cycles
double cycleCount = 0;

// Debugging variables
bool doTimingPrint = true; // Print the time between switches
bool doCyclePrint = false; // Print the number of cycles

//=============== FUNCTIONS =============

/**
 * @brief Print the latency for up and down movments for testing.
 *
 * @param switch_state The current switch state [1:down, 2:up].
 */
void printTimeBetweenSwitches(byte switch_state)
{
  unsigned long current_time = millis(); // Get the current time
  static unsigned long dt_down = 0;      // Duration from up to down
  static unsigned long dt_up = 0;        // Duration from down to up
  static unsigned long ts_last_down = 0; // Last time the down switch was triggered
  static unsigned long ts_last_up = 0;   // Last time the up switch was triggered

  // Down switch triggered
  if (switch_state == 1)
  {
    if (ts_last_up != 0) // Check if up switch was previously triggered
    {
      dt_down = current_time - ts_last_up;
      Serial.print(dt_down);
      Serial.println(";");
    }
    if (cycleCount == 100)
    {
      Serial.println("100 CYCLES");
    }
    ts_last_down = current_time; // Update last down switch trigger time
  }
  // Up switch triggered
  else if (switch_state == 2)
  {
    if (ts_last_down != 0) // Check if down switch was previously triggered
    {
      dt_up = current_time - ts_last_down;
      Serial.print(dt_up);
      Serial.print(", ");
    }
    ts_last_up = current_time; // Update last up switch trigger time
  }
}

/**
 * @brief Check the limit switches to see if they are triggered.
 *
 * @return 1 if the down limit switch is triggered, 2 if the up limit switch is triggered, 0 otherwise.
 */
byte checkLimitSwitches()
{
  static byte switch_state = 0; // [1:down, 2:up]

  // Check the limit switches
  if (digitalRead(pinDownIO) == HIGH && switch_state != 1)
  {
    // Serial.println("Down limit switch triggered");
    switch_state = 1;
    // Increment the cycle count for down movement
    cycleCount++;
    // Print the number of cycles
    if (doCyclePrint)
    {
      Serial.print("Cycles: ");
      Serial.println(cycleCount);
    }
    // Print the time between switches
    printTimeBetweenSwitches(switch_state);
  }
  else if (digitalRead(pinUpIO) == HIGH && switch_state != 2)
  {
    // Serial.println("Up limit switch triggered");
    switch_state = 2;
    // Print the time between switches
    if (doTimingPrint)
    {
      printTimeBetweenSwitches(switch_state);
    }
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

  // Move the gate down
  analogWrite(pinDownPWM, pwmDownFreq);
  // Check for the down limit switch
  while (checkLimitSwitches() != 1)
  {
    delay(10);
  }
  // Stop the gate
  analogWrite(pinDownPWM, 0);
  // Reset the count
  cycleCount = 0;
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