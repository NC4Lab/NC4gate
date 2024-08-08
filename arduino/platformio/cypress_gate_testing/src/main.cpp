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

// CUSTOM
#include <GateDebug.h>
#include <CypressCom.h>
#include <SerialCom.h>
#include <GateOperation.h>

//============ VARIABLES ===============

// Global variables
bool DB_VERBOSE = 1;  //< set to control debugging behavior [0:silent, 1:verbose]
bool DO_ECAT_SPI = 1; //< set to control block SPI [0:dont start, 1:start]

// Gate operation setup
uint8_t pwmDuty = 255;         // PWM duty for all walls [0-255]
uint16_t dtMoveTimeout = 2000; // timeout for wall movement (ms)

//

// Initialize class instances for local libraries
GateDebug Dbg;                                  // Debugging class
GateOperation WallOper(pwmDuty, dtMoveTimeout); // Wall operation class

//=============== FUNCTIONS =============
void runWalls(uint8_t wall_ind)
{
  // Set bit in wall byte to bit_val
  uint8_t byte_wall_state_new;
  bitWrite(byte_wall_state_new, wall_ind, 1);
  // Set walls to move up for this chamber and run
  WallOper.setWallsToMove(0, byte_wall_state_new);
  WallOper.moveWallsConductor();

  // Move walls back down
  WallOper.setWallsToMove(0, 0);
  WallOper.moveWallsConductor();
}

//=============== SETUP =================
void setup()
{
  // Setup serial coms for serial monoitor
  /// @note Comment this out if SerialCom is using the "Serial" HardwareSerial port
  Serial.begin(115200);
  delay(100);

  Dbg.printMsg(Dbg.MT::HEAD1, "UPLOADING TO ARDUNO...");

  // Initialize I2C for Cypress chips
  WallOper.CypCom.i2cInit();

  // Scan I2C bus for Cypress chips and store and print found addresses
  WallOper.CypCom.i2cScan();

  // Initialize wall operation
  WallOper.initGateOperation();

  // Initialize cypress chips
  WallOper.initCypress();

  // Print which microcontroller is active
  Dbg.printMsg(Dbg.MT::HEAD2, "FINISHED SETUP");
}

//=============== LOOP ==================
void loop()
{
  runWalls(0);
  while (true)
  {
    /* code */
  }
  
  // // Set walls to move up for this chamber
  // WallOper.setWallsToMove(0, byte_wall_state_new);

  // // Run move walls operation
  // WallOper.moveWallsConductor();
}