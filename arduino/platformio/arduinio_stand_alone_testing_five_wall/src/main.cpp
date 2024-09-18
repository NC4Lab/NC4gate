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
  static byte switch_state[NWALLS] = {0}; // [1:down, 2:up]

  // Check the down switch
  if (digitalRead(pinDownIO[i_wall]) == HIGH && switch_state[i_wall] != 1)
  {
    switch_state[i_wall] = 1;
    // Increment the cycle count for down movement
    cycleCount[i_wall]++;

    // // TEMP
    // Serial.print("Down switch triggered: ");
    // Serial.println(i_wall);
  }
  // Check the up switch
  else if (digitalRead(pinUpIO[i_wall]) == HIGH && switch_state[i_wall] != 2)
  {
    switch_state[i_wall] = 2;

    // // TEMP
    // Serial.print("UP switch triggered: ");
    // Serial.println(i_wall);
  }
  else
  {
    return 0;
  }
  return switch_state[i_wall];
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
  for (int i = 0; i < NWALLS; i++)
  {
    if (run_dir == 1)
    {
      // Move the wall down
      analogWrite(pinDownPWM[i], pwmDownFreq);
    }
    else if (run_dir == 2)
    {
      // Move the wall up
      analogWrite(pinUpPWM[i], pwmUpFreq);
    }
  }

  // Check each wall, with a 2.5-second timeout
  while (millis() - start_time < 2500)
  {
    for (int i = 0; i < NWALLS; i++)
    {
      if (is_ran_arr[i] == true)
      {
        continue;
      }
      if (run_dir == 1)
      {
        // Check for the down limit switch
        if (checkLimitSwitches(i) == 1)
        {
          // Stop the wall
          analogWrite(pinUpPWM[i], 0);
          analogWrite(pinDownPWM[i], 0);

          is_ran_arr[i] = true;
        }
      }
      else if (run_dir == 2)
      {
        // Check for the up limit switch
        if (checkLimitSwitches(i) == 2)
        {
          // Stop the wall
          analogWrite(pinUpPWM[i], 0);
          analogWrite(pinDownPWM[i], 0);

          is_ran_arr[i] = true;
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
  for (int i = 0; i < NWALLS; i++)
  {
    pinMode(pinDownIO[i], INPUT);
    pinMode(pinUpIO[i], INPUT);
  }

  // Initialize the PWM pins
  for (int i = 0; i < NWALLS; i++)
  {
    pinMode(pinUpPWM[i], OUTPUT);
    pinMode(pinDownPWM[i], OUTPUT);
    analogWrite(pinDownPWM[i], 0);
    analogWrite(pinUpPWM[i], 0);
  }

  // Move the gate down
  runWalls(1);
  delay(1000);

  // Reset the cycle count
  for (int i = 0; i < NWALLS; i++)
  {
    cycleCount[i] = 0;
  }

  while (true)
    ;
}

//=============== LOOP ==================
void loop()
{
  // // TEMP
  // for (int i = 0; i < NWALLS; i++)
  // {
  //   checkLimitSwitches(i);
  // }
  // return;

  // Run wall up
  runWalls(2);

  // Run wall down
  runWalls(1);

  // Print the cycles
  printCycleCounts();
}