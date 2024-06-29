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
    const unsigned long TIMEOUT = 1500; // Timeout for receiving messages (ms)
    GateDebug _Dbg; // Local instance of GateDebug class

public:
    // Struct for message data
    struct MessageData
    {
        byte msg_type; // Message type
        byte data[200];  // Message data
        byte length;   // Message length
    };
    MessageData MD; // only one instance used

    // ---------------METHODS---------------

public:
    SerialCom(HardwareSerial &serial);

public:
    void begin(unsigned long baud);

public:
    bool receiveMessage();

public:
    void sendMessage(byte msg_type, const byte *message_data, size_t length);

private:
    byte _calculateChecksum(const byte *data_array, size_t length);

private:
    byte _readByte();
};

#endif // SERIALCOM_H