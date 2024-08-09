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
bool DB_VERBOSE = 0;  //< set to control debugging behavior [0:silent, 1:verbose]
bool DO_ECAT_SPI = 1; //< set to control block SPI [0:dont start, 1:start]

// Gate operation setup
uint8_t pwmDuty = 255;         // PWM duty for all walls [0-255]
uint16_t dtMoveTimeout = 1000; // timeout for wall movement (ms)

// Indeces of walls to test
uint8_t wallInds[5] = {0, 1, 2, 3, 4};

// Track the number of cycles
unsigned long cycleCount[5] = {0, 0, 0, 0, 0};

// Track response from wall operation
uint8_t runStatus[5] = {1, 1, 1, 1, 1};

// Track timing
unsigned long tsUp[5] = {0, 0, 0, 0, 0};
unsigned long tsDown[5] = {0, 0, 0, 0, 0};
unsigned long dtUp[5] = {0, 0, 0, 0, 0};
unsigned long dtDown[5] = {0, 0, 0, 0, 0};

// Initialize class instances for local libraries
GateDebug Dbg;                                  // Debugging class
GateOperation WallOper(pwmDuty, dtMoveTimeout); // Wall operation class

//=============== FUNCTIONS =============

/**
 * @brief Run walls up and down for a given chamber.
 *
 * @param wall_ind Index of the wall to run.
 * @param run_dir Direciton to run [0:down, 1:up].
 * @return Status/error codes [0:no move, 1:success, 2:i2c error, 3:timeout] or [-1=255:input argument error].
 */
bool runWalls(uint8_t wall_ind, uint8_t run_dir)
{

  // Run walls up
  if (run_dir == 1)
  {

    // Set walls to move up
    uint8_t byte_wall_state_new = 0;
    bitWrite(byte_wall_state_new, wall_ind, 1);

    // Set walls to move up and run
    DB_VERBOSE = 0;
    WallOper.setWallsToMove(0, byte_wall_state_new);
    runStatus[wall_ind] = WallOper.moveWallsConductor();
    DB_VERBOSE = 1;

    // Check status
    if (runStatus[wall_ind] != 1)
    {
      Dbg.printMsg(Dbg.MT::ERROR, "Error moving wall up: wall[%d] resp[%d]", wall_ind, runStatus[wall_ind]);
      return false;
    }
  }

  // Run walls down
  if (run_dir == 0)
  {

    // Set walls to move back down and run
    DB_VERBOSE = 0;
    WallOper.setWallsToMove(0, 0);
    runStatus[wall_ind] = WallOper.moveWallsConductor();
    DB_VERBOSE = 1;

    // Check status
    if (runStatus[wall_ind] != 1)
    {
      Dbg.printMsg(Dbg.MT::ERROR, "Error moving wall down: wall[%d] resp[%d]", wall_ind, runStatus[wall_ind]);
      return false;
    }
  }

  return true;
}

/**
 * @brief Run walls up and down.
 */
void wallsCycleTest()
{
  bool is_run = false;

  // Loop throuh walls
  for (uint8_t i = 0; i < 5; i++)
  {
    unsigned long ts = millis();

    // Skip if wall has failed
    if (runStatus[wallInds[i]] != 1)
    {
      continue;
    }

    // Run wall up
    is_run = runWalls(wallInds[i], 1);

    // Run wall down
    if (is_run)
    {
      is_run = runWalls(wallInds[i], 0);
    }

    if (is_run)
    {
      cycleCount[wallInds[i]]++;
    }

    // add delay if 2 seconds have not passed
    while (millis() - ts < 2000)
      ;
  }

  // Print cycles and status
  Dbg.printMsg(Dbg.MT::INFO, "Cycle counts: w0[%lu] w1[%lu] w2[%lu] w3[%lu] w4[%lu]",
               cycleCount[0], cycleCount[1], cycleCount[2], cycleCount[3], cycleCount[4]);
  Dbg.printMsg(Dbg.MT::INFO, "Run status: w0[%d] w1[%d] w2[%d] w3[%d] w4[%d]",
               runStatus[0], runStatus[1], runStatus[2], runStatus[3], runStatus[4]);
}

void runTimingTest()
{
  // Loop throuh walls
  for (uint8_t i = 0; i < 5; i++)
  {
    // Run wall up
    runWalls(wallInds[i], 1);
    tsUp[wallInds[i]] = millis(); // Update last up switch trigger time

    // Run wall down
    runWalls(wallInds[i], 0);
    tsDown[wallInds[i]] = millis(); // Update last down switch trigger time
    dtDown[wallInds[i]] = tsDown[wallInds[i]] - tsUp[wallInds[i]];

    // Run wall up
    runWalls(wallInds[i], 1);
    dtUp[wallInds[i]] = millis() - tsDown[wallInds[i]];

    // Run wall down
    runWalls(wallInds[i], 0);

    // Increment cycle count
    cycleCount[wallInds[i]]++;
  }

  // Print timing for each wall
  Serial.print(dtUp[0]);
  Serial.print(", ");
  Serial.print(dtUp[1]);
  Serial.print(", ");
  Serial.print(dtUp[2]);
  Serial.print(", ");
  Serial.print(dtUp[3]);
  Serial.print(", ");
  Serial.print(dtUp[4]);
  Serial.print(", ");
  Serial.print(dtDown[0]);
  Serial.print(", ");
  Serial.print(dtDown[1]);
  Serial.print(", ");
  Serial.print(dtDown[2]);
  Serial.print(", ");
  Serial.print(dtDown[3]);
  Serial.print(", ");
  Serial.print(dtDown[4]);
  Serial.println(";");

  // Hold here if max cycles reached
  if (cycleCount[4] == 50)
  {
    while (true)
      ;
  }
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

  while (true)
    ;
}

//=============== LOOP ==================
void loop()
{
  // Do wall cycle test
  wallsCycleTest();

  // // Do timing test
  // runTimingTest();
}