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
uint16_t dtMoveTimeout = 2000; // timeout for wall movement (ms)

// Initialize class instances for local libraries
GateDebug Dbg;                                  // Debugging class
SerialCom SerCom(Serial);                       // Serial communication class
GateOperation WallOper(pwmDuty, dtMoveTimeout); // Wall operation class

//=============== SETUP =================
void setup()
{
  // Setup serial coms for serial monoitor
  // /// @note Comment this out if SerialCom is using the "Serial" HardwareSerial port
	// Serial.begin(115200);
	// delay(100);

  // Setup serial coms for SerialCom
  SerCom.begin(115200);

  // Initialize I2C for Cypress chips
  WallOper.CypCom.i2cInit();

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

      // Initialize wall operation
      WallOper.initGateOperation();

      // Initialize cypress chips
      WallOper.initCypress();

      // Send back found addresses
      SerCom.sendMessage(SerCom.MD.msg_type, WallOper.CypCom.listAddr, WallOper.CypCom.nAddr);
    }

    // Handle gates initialization message
    if (SerCom.MD.msg_type == 1)
    {
      // Initialize walls in up position
      WallOper.initWalls(1);

      // Store up walls as a byte array
      uint8_t msg_arg_arr[WallOper.CypCom.nAddr];
      for (size_t cyp_i = 0; cyp_i < WallOper.CypCom.nAddr; cyp_i++)
      {
        msg_arg_arr[cyp_i] = WallOper.C[cyp_i].bitWallPosition;
      }

      // Send back wall states
      SerCom.sendMessage(SerCom.MD.msg_type, msg_arg_arr, WallOper.CypCom.nAddr);

      // Initialize walls back to down position
      WallOper.initWalls(0);
    }

    // Handle move gates message
    if (SerCom.MD.msg_type == 2)
    {

      // Loop through message
      for (byte cyp_i = 0; cyp_i < SerCom.MD.length; ++cyp_i)
      {
        // Get wall byte mask data
        uint8_t byte_wall_state_new = SerCom.MD.data[cyp_i];

        // Set walls to move up for this chamber
        WallOper.setWallsToMove(cyp_i, byte_wall_state_new);
      }

      // Run move walls operation
      WallOper.moveWallsConductor();

      // Store up walls as a byte array
      uint8_t msg_arg_arr[WallOper.CypCom.nAddr];
      for (size_t cyp_i = 0; cyp_i < WallOper.CypCom.nAddr; cyp_i++)
      {
        msg_arg_arr[cyp_i] = WallOper.C[cyp_i].bitWallPosition;
      }

      // Send back wall states
      SerCom.sendMessage(SerCom.MD.msg_type, msg_arg_arr, WallOper.CypCom.nAddr);
    }
  }

  // //............... Cypress Testing ...............

  // // Test input pins
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallIO(0, a_wall, 1);

  // // Test PWM output
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallPWM(0, a_wall, 1);
  // while (true);

  // // Test wall operation
  // uint8_t a_wall[1] = { 1 };
  // WallOper.testWallOperation(0, a_wall, 1);
}