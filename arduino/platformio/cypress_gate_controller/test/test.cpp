#include "Arduino.h"
#include <Wire.h>

void setup()
{
  Serial.begin(115200); // Initialize Serial for debugging
  while (!Serial);      // Wait for Serial to initialize
  delay(100);           // Give some time to ensure the buffer is clean
  Serial.flush();       // Clear any initial garbage data
}

void loop()
{
  if (Serial.available())
  {
    char c = Serial.read(); // Read the incoming byte on Serial
    Serial.write(c);        // Echo the byte back on Serial
  }
}

// void setup() {
//   Serial1.begin(115200);  // Initialize Serial1 at 115200 baud rate
//   Serial.begin(115200);   // Initialize Serial for debugging
//   Serial.println("Arduino Ready");
// }

// void loop() {
//   if (Serial1.available()) {
//     char c = Serial1.read(); // Read the incoming byte on Serial1
//     Serial1.write(c);        // Echo the byte back on Serial1
//     Serial.print(c);         // Print the byte to Serial for debugging
//   }
// }
