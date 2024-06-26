// ######################################

// ============== main.ino ==============

// ######################################

/**
 * @file Main Arduino INO file for running the maze.
 */

// BUILT IN
#include "Arduino.h"
#include <Wire.h>

// CUSTOM
#include <EsmacatCom.h>
#include <GateDebug.h>
#include <CypressCom.h>
#include <SerialCom.h>
#include <WallOperation.h>

//============ VARIABLES ===============

// Global variables
bool DB_VERBOSE = 1;  //< set to control debugging behavior [0:silent, 1:verbose]
bool DO_ECAT_SPI = 1; //< set to control block SPI [0:dont start, 1:start]

// Wall opperation setup (these will be overwritten by the Ethercat message)
uint8_t nCham = 1;             // number of chambers being used [1-9]
uint8_t nChamPerBlock = 1;     // max number of chambers to move at once [1-nCham]
uint8_t nMoveAttempt = 1;      // number of attempts to move a walls [1-255]
uint8_t pwmDuty = 255;         // PWM duty for all walls [0-255]
uint16_t dtMoveTimeout = 1000; // timeout for wall movement (ms)

// // Initialize class instances for local libraries
GateDebug Dbg;
CypressCom CypCom;
SerialCom SerCom(Serial1);
WallOperation WallOper(nCham, nChamPerBlock, nMoveAttempt, pwmDuty, dtMoveTimeout);

//=============== SETUP =================
void setup()
{
  // Setup serial coms for serial monoitor
  Serial.begin(115200);
  delay(100);
  Serial.print('\n');

  // Setup serial coms for SerialCom
  SerCom.begin(115200);

  // Print setup started
  Dbg.printMsg(Dbg.MT::HEAD1, "RUNNING SETUP");

  // Initialize I2C for Cypress chips
  CypCom.i2cInit();
  while(true);

// Print which microcontroller is active
#ifdef ARDUINO_AVR_UNO
  Dbg.printMsg(Dbg.MT::HEAD1, "FINISHED UPLOADING TO ARDUNO UNO");
#endif
#ifdef __AVR_ATmega2560__
  Dbg.printMsg(Dbg.MT::HEAD1, "FINISHED UPLOADING TO ARDUNO MEGA");
#endif
#ifdef ARDUINO_SAM_DUE
  Dbg.printMsg(Dbg.MT::HEAD1, "FINISHED UPLOADING TO ARDUNO DUE");
#endif
}

//=============== LOOP ==================
void loop()
{
  String receivedMessage;

  // Check if a message is received
  if (SerCom.receiveMessage(receivedMessage))
  {
    // Print the received message to the Serial Monitor
    Serial.print("Received: ");
    Serial.println(receivedMessage);

    // Process the received message (for example, toggle an LED)
    if (receivedMessage == "Q")
    {
      Dbg.printMsg(Dbg.MT::INFO, "Query Acknowledged"); 
    }
    else
    {
      Dbg.printMsg(Dbg.MT::INFO, "Unknown Command");
    }
  }

  
  // while (Serial1.available())
  // {
  //   Serial.write(Serial1.read());
  // }

  //............... ROS Controlled ...............

  // // Check ethercat coms
  // WallOper.EsmaCom.readEcatMessage();

  // // Process and exicute ethercat arguments
  // WallOper.procEcatMessage();

  // //............... Standalone Setup ...............
  // static bool init = 0;
  // if (!init)
  // {
  //   init = 1;
  //   Dbg.printMsg(Dbg.MT::HEAD1, "RUNNNING: STANDALONE SETUP");

  //   // Initialize software
  //   WallOper.initSoftware(0);

  //   // Initalize Cypress Chips
  //   WallOper.initCypress();

  //   // Initalize Walls
  //   WallOper.initWalls(0); // Run wall up and down

  //   Dbg.printMsg(Dbg.MT::HEAD1B, "FINISHED: STANDALONE SETUP");
  // }

  // //............... Cypress Testing ...............

  // // Test input pins
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallIO(0, a_wall, 1);

  // // Test PWM output
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallPWM(0, a_wall, 1);
  // while (true);

  // // Test wall opperation
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallOperation(0, a_wall, 1);
}