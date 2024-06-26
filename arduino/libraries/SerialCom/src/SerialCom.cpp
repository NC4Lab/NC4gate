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
}

/// @brief Receives a message from the serial port.
///
/// This function attempts to read a message from the serial port. It looks for a start byte,
/// reads the message length, the message itself, and the checksum. If the checksum is valid,
/// it acknowledges the receipt and returns true. Otherwise, it returns false.
///
/// @param message: Reference to a String where the received message will be stored.
///
/// @return true if a valid message is received and checksum is correct, otherwise false.
bool SerialCom::receiveMessage(String &message)
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
                byte length = _readByte(); // Read the length of the message
                String received_message = "";

                // Read the message content
                for (byte i = 0; i < length; ++i)
                {
                    if (serial.available())
                    {
                        received_message += (char)_readByte();
                    }
                }

                // Read and calculate the checksum
                byte checksum_expected = _readByte();
                byte checksum_calculated = _calculateChecksum(received_message);

                // Check if the end byte is correct
                byte end_byte = _readByte();
                if (end_byte == END_BYTE)
                {
                    // Validate the checksum
                    if (checksum_expected == checksum_calculated)
                    {
                        message = received_message; // Store the valid message
                        sendMessage(message);       // Acknowledge receipt
                        return true;                // Return true for a valid message
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

                _Dbg.printMsg(_Dbg.MT::WARNING, "Failed receive: start_byte[%s] length[%d] received_message[%s] checksum[%d|%d] end_byte[%s]",
                              _Dbg.hexStr(start_byte), length, received_message.c_str(), checksum_calculated, checksum_expected, _Dbg.hexStr(end_byte));
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
/// This function constructs a message with a start byte, message length, message content,
/// and checksum, then sends it over the serial port.
///
/// @param message: The message to be sent.
void SerialCom::sendMessage(const String &message)
{
    serial.write(START_BYTE);                  // Write the start byte
    serial.write(message.length());            // Write the length of the message
    serial.print(message);                     // Write the message content
    serial.write(_calculateChecksum(message)); // Write the checksum
    serial.write(END_BYTE);                    // Write the start byte
}

/// @brief Calculates the checksum for a given message.
///
/// The checksum is calculated as the XOR of all characters in the message.
///
/// @param message: The message for which to calculate the checksum.
///
/// @return The calculated checksum.
byte SerialCom::_calculateChecksum(const String &message)
{
    byte checksum = 0;

    // Calculate the checksum by summing all characters in the message
    for (char c : message)
    {
        checksum += c;
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
