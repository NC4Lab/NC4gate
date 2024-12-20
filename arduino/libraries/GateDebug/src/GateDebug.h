// ######################################

//=========== GateDebug.h =============

// ######################################

/// @file Used for the GateDebug class

//============= INCLUDE ================
#include "Arduino.h"

#ifndef _MAZE_DEBUG_h
#define _MAZE_DEBUG_h

extern bool DB_VERBOSE; ///< set this variable in your INO file to control debugging [0:silent, 1:verbose]

/// @brief Used for printing different types of information to the Serial Output Window.
///
/// @remarks This class is used in both the CypressComm and GateOperation classes.
class GateDebug
{

	// ---------------VARIABLES---------------
public:
	const char _message_type_str[8][10] = {
		"[INFO]",
		"[INFO]",
		"[INFO]",
		"[INFO]",
		"[INFO]",
		"[ERROR]",
		"[WARNING]",
		"[DEBUG]"};

	enum MT
	{
		HEAD1 = 0,
		HEAD1A = 1,
		HEAD1B = 2,
		HEAD2 = 3,
		INFO = 4,
		ERROR = 5,
		WARNING = 6,
		DEBUG = 7
	};

	// ---------------METHODS---------------
public:
	GateDebug();

public:
	void printMsg(MT, const char *, ...);

private:
	const char *_timeStr(uint32_t);

public:
	const char *arrayStr(uint8_t[], size_t);

public:
	const char *binStr(uint8_t);

public:
	const char *hexStr(uint8_t);

public:
	const char *dtTrack(bool = false);

public:
	const char *bitIndStr(uint8_t);
};

#endif
