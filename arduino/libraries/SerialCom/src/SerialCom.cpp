// ######################################

//========== SerialCom.cpp ===========

// ######################################

//============= INCLUDE ================
#include "SerialCom.h"

/// @brief Initializes the SerialCom object with the specified serial port.
///
/// @param serial: Reference to the HardwareSerial object to be used for communication.
SerialCom::SerialCom(HardwareSerial &serial) : serial(serial) {}

/// @brief Begins serial communication with the specified baud rate.
///
/// @param baud: The baud rate for serial communication.
void SerialCom::begin(unsigned long baud)
{
    serial.begin(baud);

    // Clear the buffer
    while (serial.available())
    {
        serial.read();
    }
}

/// @brief Receives a message from the serial port.
///
/// This function attempts to read a message from the serial port. It looks for a start byte,
/// reads the message length, the message itself, and the checksum. If the checksum is valid,
/// it acknowledges the receipt and returns true. Otherwise, it returns false.
///
/// @return true if a valid message is received and checksum is correct, otherwise false.
bool SerialCom::receiveMessage()
{
    // Check if data is available on the serial port
    if (serial.available())
    {
        // Read the start byte
        byte start_byte = _readByte();

        // Check if the start byte is correct
        if (start_byte == START_BYTE)
        {
            if (serial.available())
            {
                // Read and store the message type
                MD.msg_type = _readByte();

                // Read the length of the message
                MD.length = _readByte();

                // Read the message content
                MD.data[MD.length] = {};
                for (byte i = 0; i < MD.length; ++i)
                {
                    if (serial.available())
                    {
                        MD.data[i] = _readByte();
                    }
                }

                // Read and calculate the checksum
                byte checksum_expected = _readByte();
                byte checksum_calculated = _calculateChecksum(MD.data, MD.length);

                // Check if the end byte is correct
                byte end_byte = _readByte();
                if (end_byte == END_BYTE)
                {
                    // Validate the checksum
                    if (checksum_expected == checksum_calculated)
                    {
                        _Dbg.printMsg(_Dbg.MT::INFO, "Received: start_byte[%s] msg_type[%d] length[%d] data%s checksum[%d|%d] end_byte[%s]",
                                      _Dbg.hexStr(start_byte), MD.msg_type, MD.length, _Dbg.arrayStr(MD.data, MD.length), checksum_calculated, checksum_expected, _Dbg.hexStr(end_byte));
                        return true; // Return true for a valid message
                    }
                    else
                    {
                        // Discard the received message if the checksum is incorrect
                        _Dbg.printMsg(_Dbg.MT::WARNING, "Invalid checksum");
                    }
                }
                else
                {
                    // Discard the received byte if it is not the end byte
                    _Dbg.printMsg(_Dbg.MT::WARNING, "Missing end byte");
                }

                // Print failed receive message
                _Dbg.printMsg(_Dbg.MT::WARNING, "Failed receive: start_byte[%s] msg_type[%d] length[%d] data%s checksum[%d|%d] end_byte[%s]",
                              _Dbg.hexStr(start_byte), MD.msg_type, MD.length, _Dbg.arrayStr(MD.data, MD.length), checksum_calculated, checksum_expected, _Dbg.hexStr(end_byte));
            }
        }
        else
        {
            // Discard the received byte if it is not the start byte
            _Dbg.printMsg(_Dbg.MT::WARNING, "Missing start byte");
        }

        // Clear the buffer
        while (serial.available())
        {
            serial.read();
        }
    }

    // Return false if no valid message is received
    return false;
}

/// @brief Sends a message over the serial port.
///
/// This function constructs a message with a start byte, message type, message length,
/// message content, and checksum, then sends it over the serial port.
///
/// @param msg_type: The type of the message to be sent.
/// @param message_data: Pointer to the byte array containing the message to be sent.
/// @param length: The length of the byte array.
void SerialCom::sendMessage(byte msg_type, const byte *message_data, size_t length)
{
    serial.write(START_BYTE);                                 // Write the start byte
    serial.write(msg_type);                                   // Write the message type
    serial.write(static_cast<byte>(length));                  // Write the length of the message
    serial.write(message_data, length);                       // Write the message content
    byte checksum = _calculateChecksum(message_data, length); // Compute the checksum
    checksum = (checksum + msg_type) % 256;                   // Include msg_type in checksum calculation
    serial.write(checksum);                                   // Write the checksum
    serial.write(END_BYTE);                                   // Write the end byte

    // Print sent message
    _Dbg.printMsg(_Dbg.MT::INFO, "Sent: msg_type[%d] length[%d] data%s checksum[%d]",
                  msg_type, length, _Dbg.arrayStr(message_data, length), checksum);
}

/// @brief Calculates the checksum for a given message.
///
/// The checksum is calculated as the sum of all bytes in the message, modulo 256.
///
/// @param data_array: Pointer to the byte array for which to calculate the checksum.
/// @param length: The length of the byte array.
///
/// @return The calculated checksum.
byte SerialCom::_calculateChecksum(const byte *data_array, size_t length)
{
    byte checksum = 0;

    // Calculate the checksum by summing all bytes in the message
    for (size_t i = 0; i < length; ++i)
    {
        checksum += data_array[i];
    }
    return checksum % 256; // Return the calculated checksum modulo 256
}

/// @brief Read the next byte in the serial buffer.
///
/// The checksum is calculated as the XOR of all characters in the message.
///
/// @param message: The message for which to calculate the checksum.
///
/// @return The read byte.
byte SerialCom::_readByte()
{
    unsigned long start_time = millis(); // Record the start time

    // Wait for data to be available or until timeout
    while (!serial.available())
    {
        if (millis() - start_time >= TIMEOUT)
        {
            _Dbg.printMsg(_Dbg.MT::WARNING, "Serial read timed out");
            return false; // Timeout occurred
        }
    }

    // Read and return the byte from the serial buffer
    byte b = serial.read();
    delay(1); // Hack to fix issue with reading bytes
    return b;
}
