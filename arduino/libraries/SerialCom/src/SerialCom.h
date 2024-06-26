// ######################################

//=========== SerialCom.h ============

// ######################################
#ifndef SERIALCOM_H
#define SERIALCOM_H

//============= INCLUDE ================
#include "Arduino.h"
#include "GateDebug.h"

/// @brief This class handles the serial communication for running the gates.
///
class SerialCom
{

    // --------------VARIABLES--------------

private:
    HardwareSerial &serial;             // Reference to the serial port
    const byte START_BYTE = 0x02;       // Start byte for messages
    const byte END_BYTE = 0x03;         // End byte for messages
    const unsigned long TIMEOUT = 1000; // 1 second timeout for receiving messages
    GateDebug _Dbg;                     // local instance of GateDebug class

    // ---------------METHODS---------------

public:
    SerialCom(HardwareSerial &serial);

public:
    void begin(unsigned long baud);

public:
    bool receiveMessage(String &message);

public:
    void sendMessage(const String &message);

private:
    byte _calculateChecksum(const String &message);

private:
    byte _readByte();
};

#endif // SERIALCOM_H