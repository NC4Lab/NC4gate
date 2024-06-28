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

// Wall opperation setup
uint8_t pwmDuty = 255;         // PWM duty for all walls [0-255]
uint16_t dtMoveTimeout = 1000; // timeout for wall movement (ms)

// // Initialize class instances for local libraries
GateDebug Dbg;
SerialCom SerCom(Serial1);
WallOperation WallOper(pwmDuty, dtMoveTimeout);

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
  WallOper.CypCom.i2cInit();

  // // Scan I2C bus for Cypress chips and store and print found addresses
  // WallOper.CypCom.i2cScan();
  // while(true);

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
  // Check if a message is received
  if (SerCom.receiveMessage())
  {
    // Print the received message to the Serial Monitor
    Dbg.printMsg(Dbg.MT::INFO, "Received message: type[%d]", SerCom.MD.msg_type);

    // Handle Cypress initialization message
    if (SerCom.MD.msg_type == 0)
    {
      // Scan I2C bus for Cypress chips and store and print found addresses
      WallOper.CypCom.i2cScan();

      // Initialize wall opperation
      WallOper.initWallOper();

      // Initialize cypress chips
      WallOper.initCypress();

      // Send back found addresses
      SerCom.sendMessage(SerCom.MD.msg_type, WallOper.CypCom.listAddr, WallOper.CypCom.nAddr);
    }

    // Handle gatews initialization message
    if (SerCom.MD.msg_type == 1)
    {
      // Initialize walls in up position
      WallOper.initWalls(1);

      // Store active walls as a byte array
      uint8_t msg_arg_arr[WallOper.CypCom.nAddr];
      for (size_t cyp_i = 0; cyp_i < WallOper.CypCom.nAddr; cyp_i++)
      {
        msg_arg_arr[cyp_i] = WallOper.C[cyp_i].bitWallPosition;
      }

      // Send back found addresses
      SerCom.sendMessage(SerCom.MD.msg_type, msg_arg_arr, WallOper.CypCom.nAddr);

      // Initialize walls back to down position
      WallOper.initWalls(0);
    }
  }

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