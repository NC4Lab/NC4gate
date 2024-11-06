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

// Flags for testing
bool doLimitSwitchCheck = false;
bool doLimitSwitchPrint = false;
bool doRunStatePrint = false;
bool doTimePrint = false;

// Timing variables
const unsigned long dtRun = 5000;     // Minimum delay between loops
const unsigned long dtTimeout = 2000; // Timeout for the limit switch check

// Number of walls
const int NWALLS = 5;

// Pin mapping
int pinDownPWM[5] = {2, 4, 6, 8, 10};    // Down PWM pin (purple)
int pinUpPWM[5] = {3, 5, 7, 9, 11};      // Up PWM pin (blue wire)
int pinUpIO[5] = {23, 25, 27, 29, 31};   // Up limit switch pin (yellow wire)
int pinDownIO[5] = {22, 24, 26, 28, 30}; // Down limit switch pin (green wire)

// PWM settings
uint8_t pwmUpFreq = 255;   // PWM dowm movement frequency [0, 255]
uint8_t pwmDownFreq = 255; // PWM up movement frequency [0, 255]

// Limit switch state
byte switchState[5] = {0, 0, 0, 0, 0}; // [1:down, 2:up]

// Track the number of cycles
unsigned long cycleCount[5] = {0, 0, 0, 0, 0};

//=============== FUNCTIONS =============

/**
 * @brief Check the limit switches to see if they are triggered.
 *
 * @return 1 if the down limit switch is triggered, 2 if the up limit switch is triggered, 0 otherwise.
 */
byte checkLimitSwitches(int i_wall)
{
  // Check the down switch
  if (digitalRead(pinDownIO[i_wall]) == HIGH && switchState[i_wall] != 1)
  {
    switchState[i_wall] = 1;

    // Print switch triggered
    if (doLimitSwitchCheck || doLimitSwitchPrint)
    {
      Serial.print("Down switch triggered: ");
      Serial.println(i_wall);
    }
  }
  // Check the up switch
  else if (digitalRead(pinUpIO[i_wall]) == HIGH && switchState[i_wall] != 2)
  {
    switchState[i_wall] = 2;

    // Print switch triggered
    if (doLimitSwitchCheck || doLimitSwitchPrint)
    {
      Serial.print("UP switch triggered: ");
      Serial.println(i_wall);
    }
  }
  else
  {
    return 0;
  }
  return switchState[i_wall];
}

/**
 * @brief Run the walls up or down.
 *
 * @param run_dir Direction to run [1:down, 2:up].
 */
void runWalls(int run_dir)
{
  bool is_ran_arr[NWALLS] = {0};
  unsigned long start_time = millis(); // Record the start time

  // Run each wall
  for (int i_wall = 0; i_wall < NWALLS; i_wall++)
  {
    if (run_dir == 1)
    {
      // Move the wall down
      analogWrite(pinUpPWM[i_wall], 0);
      analogWrite(pinDownPWM[i_wall], pwmDownFreq);

      if (doRunStatePrint)
      {
        Serial.print("Start run wall down: wall=");
        Serial.print(i_wall);
        Serial.print(" state=");
        Serial.println(switchState[i_wall]);
      }
    }
    else if (run_dir == 2)
    {
      // Move the wall up
      analogWrite(pinDownPWM[i_wall], 0);
      analogWrite(pinUpPWM[i_wall], pwmUpFreq);

      if (doRunStatePrint)
      {
        Serial.print("Start run wall up: wall=");
        Serial.print(i_wall);
        Serial.print(" state=");
        Serial.println(switchState[i_wall]);
      }
    }
  }

  // Check each wall, with a timeout
  while (millis() - start_time < dtTimeout)
  {
    for (int i_wall = 0; i_wall < NWALLS; i_wall++)
    {
      if (is_ran_arr[i_wall])
      {
        continue;
      }
      if (run_dir == 1)
      {
        // Check for the down limit switch
        if (checkLimitSwitches(i_wall) == 1)
        {
          // Increment the cycle count for down movement
          cycleCount[i_wall]++;

          // Stop the wall
          analogWrite(pinUpPWM[i_wall], 0);
          analogWrite(pinDownPWM[i_wall], 0);

          is_ran_arr[i_wall] = true;

          if (doRunStatePrint)
          {
            Serial.print("Stop run wall down: wall=");
            Serial.print(i_wall);
            Serial.print(" state=");
            Serial.println(switchState[i_wall]);
          }
        }
      }
      else if (run_dir == 2)
      {
        // Check for the up limit switch
        if (checkLimitSwitches(i_wall) == 2)
        {
          // Stop the wall
          analogWrite(pinUpPWM[i_wall], 0);
          analogWrite(pinDownPWM[i_wall], 0);

          is_ran_arr[i_wall] = true;

          if (doRunStatePrint)
          {
            Serial.print("Stop run wall up: wall=");
            Serial.print(i_wall);
            Serial.print(" state=");
            Serial.println(switchState[i_wall]);
          }
        }
      }
    }
  }
}

/**
 * @brief Print the number of cycles.
 */
void printCycleCounts()
{
  char buffer[100]; // Create a buffer to hold the formatted string

  sprintf(buffer, "Cycle counts: w0[%lu] w1[%lu] w2[%lu] w3[%lu] w4[%lu]",
          cycleCount[0], cycleCount[1], cycleCount[2], cycleCount[3], cycleCount[4]);

  Serial.println(buffer); // Print the formatted string
}

//=============== SETUP =================
void setup()
{
  // Setup serial coms for serial monoitor
  Serial.begin(115200);
  delay(100);

  // Initialize the limit switches
  for (int i_wall = 0; i_wall < NWALLS; i_wall++)
  {
    pinMode(pinDownIO[i_wall], INPUT);
    pinMode(pinUpIO[i_wall], INPUT);
  }

  // Initialize the PWM pins
  for (int i_wall = 0; i_wall < NWALLS; i_wall++)
  {
    pinMode(pinUpPWM[i_wall], OUTPUT);
    pinMode(pinDownPWM[i_wall], OUTPUT);
    analogWrite(pinDownPWM[i_wall], 0);
    analogWrite(pinUpPWM[i_wall], 0);
  }

  // Move the gate down
  if (!doLimitSwitchCheck)
  {
    runWalls(1);
    delay(1000);
  }

  // Reset the cycle count
  for (int i_wall = 0; i_wall < NWALLS; i_wall++)
  {
    cycleCount[i_wall] = 0;
  }
}

//=============== LOOP ==================
void loop()
{
  static unsigned long previous_millis = 0; // Store the last time the loop ran
  unsigned long current_millis = millis();  // Get the current time
  unsigned long loop_dt = current_millis - previous_millis;

  // Run limit switch test
  if (doLimitSwitchCheck)
  {
    for (int wall_i = 0; wall_i < NWALLS; wall_i++)
    {
      checkLimitSwitches(wall_i);
    }
    return;
  }

  // Check if enough time has passed
  if (loop_dt < dtRun)
  {
    return;
  }

  // Update the last execution time
  previous_millis = current_millis;

  // Run wall up
  runWalls(2);

  // Run wall down
  runWalls(1);

  // Print the cycles
  printCycleCounts();

  // Print the time
  if (doTimePrint)
  {
    Serial.print("Time elapsed: total=");
    Serial.print(current_millis);
    Serial.print(" loop=");
    Serial.println(loop_dt);
  }
}