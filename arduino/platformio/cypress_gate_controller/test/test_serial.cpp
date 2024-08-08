// ######################################

// ============== main.cpp ==============

// ######################################

/**
 * @file Main Arduino file for running the maze.
 *
 * @note Set DB_VERBOSE to 0 and use the "Serial" HardwareSerial port
 * for SerCom(Serial) initialation if you are performing serial
 * communication with the Arduino USB connection.
 */

// BUILT IN
#include "Arduino.h"
#include <Wire.h>

void setup() {
  Serial1.begin(115200);  // Initialize Serial1 at 115200 baud rate
  Serial.begin(115200);   // Initialize Serial for debugging
  Serial.println("Arduino Ready");
}

void loop() {
  if (Serial1.available()) {
    char c = Serial1.read(); // Read the incoming byte on Serial1
    Serial1.write(c);        // Echo the byte back on Serial1
    Serial.print(c);         // Print the byte to Serial for debugging
  }
}
