// ######################################

//========== CypressCom.h ============

// ######################################

/// @file Used for the CypressCom class

#ifndef _CYPRESS_COMM_h
#define _CYPRESS_COMM_h

//============= INCLUDE ================
#include "Arduino.h"
#include <Wire.h>
#include "CypressComBase.h"
#include "GateDebug.h"

/// @brief This class handles all of the Cypress chip I2C comms.
///
/// @remarks This class uses an instance of the GateDebug class.
///
/// @remarks Status codes from @ref Wire::beginTransmission()
/// 	0: Success. Indicates that the transmission was successful.
/// 	1: Data too long to fit in transmit buffer. This error indicates that the data you tried
///			to send is larger than the library's buffer.
/// 	2: Received NACK on transmit of address. This means the slave device did not acknowledge its address.
/// 		This can happen if the device is not connected or if there is an issue with the I2C address.
/// 	3: Received NACK on transmit of data. This means the slave device acknowledged its address but
///			did not acknowledge the receipt of data.
/// 	4: Other error. A miscellaneous error occurred.
///		5: Timeout. The transmission timed out.
class CypressCom
{

	// ---------VARIABLES-----------------
public:
	// Global address variable
	uint8_t nowAddr = 0; /// tracks current I2C address for debugging
	uint8_t listAddr[9]; /// List of cypress up to 9 I2C addresses
	uint8_t nAddr = 0; /// Number of cypress I2C addresses found

	// PWM config
	const uint8_t pwmClockVal = 0;	 /// PWM clock config [0: 32 kHz(default), 1: 24 MHz, 2: 1.5 MHz, 3: 93.75 kHz, 4: 367.6 Hz(programmable), 5: previous PWM]
	const uint8_t pwmPeriodVal = 32; /// PWM period of the PWM counter(1 - 255).Devisor for hardward clock

private:
	GateDebug _Dbg; /// unique instance of GateDebug class

	// -----------METHODS-----------------
public:
	CypressCom();

public:
	uint8_t i2cScan();

public:
	uint8_t i2cInit();

public:
	uint8_t i2cRead(uint8_t, uint8_t, uint8_t[], uint8_t = 1);

public:
	uint8_t i2cWrite(uint8_t, uint8_t, uint8_t);
	uint8_t i2cWrite(uint8_t, uint8_t, uint8_t[], uint8_t);

private:
	void _updateRegByte(uint8_t &, uint8_t, uint8_t);

public:
	uint8_t ioReadPin(uint8_t, uint8_t, uint8_t, uint8_t &);

public:
	uint8_t ioWritePin(uint8_t, uint8_t, uint8_t, uint8_t);

public:
	uint8_t ioReadPort(uint8_t, uint8_t, uint8_t, uint8_t &);

public:
	uint8_t ioWritePort(uint8_t, uint8_t, uint8_t, uint8_t);

public:
	uint8_t ioReadReg(uint8_t, uint8_t, uint8_t[], uint8_t);

public:
	uint8_t ioWriteReg(uint8_t, uint8_t[], uint8_t, uint8_t, uint8_t[] = nullptr);

public:
	uint8_t setupCypress(uint8_t);

public:
	uint8_t setupSourcePWM(uint8_t, uint8_t, uint8_t);

public:
	uint8_t setSourceDutyPWM(uint8_t, uint8_t, uint8_t);

public:
	uint8_t setPortRegister(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

private:
	void _beginTransmissionWrapper(uint8_t);

private:
	uint8_t _endTransmissionWrapper(bool = true, bool = true);

public:
	void printRegByte(uint8_t);
	void printRegByte(uint8_t[], uint8_t);
};

#endif
