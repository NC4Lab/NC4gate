// ######################################

//======== GateOperation.cpp ==========

// ######################################

//============= INCLUDE ================
#include "GateOperation.h"

//======== CLASS: WALL_OPERATION ==========

/// @brief CONSTUCTOR: Create GateOperation class instance
///
/// @param _nCham: Spcify number of cypress boards to track [1-49]
/// @param _pwmDuty: Defualt duty cycle for all the pwm [0-255]
GateOperation::GateOperation(uint8_t _pwmDuty, uint16_t _dtMoveTimeout)
{
	// Store input variables
	CypCom.nAddr = 0;
	pwmDuty = _pwmDuty;
	dtMoveTimeout = _dtMoveTimeout;

	// Create WallMapStruct lists for each function
	_makePMS(pmsAllIO, wms.ioDown[0], wms.ioDown[1], wms.ioUp[0], wms.ioUp[1]);		 // all io pins
	_makePMS(pmsAllPWM, wms.pwmDown[0], wms.pwmDown[1], wms.pwmUp[0], wms.pwmUp[1]); // all pwm pins
	_makePMS(pmsUpIO, wms.ioUp[0], wms.ioUp[1]);									 // io up pins
	_makePMS(pmsDownIO, wms.ioDown[0], wms.ioDown[1]);								 // io down pins
	_makePMS(pmsUpPWM, wms.pwmUp[0], wms.pwmUp[1]);									 // pwm up pins
	_makePMS(pmsDownPWM, wms.pwmDown[0], wms.pwmDown[1]);							 // pwm down pins

	// Update pin function map [0,1,2,3] [io down, io up, pwm down, pwm up]
	for (size_t i = 0; i < 8; i++)
	{														  // loop wall map entries
		wms.funMap[wms.ioDown[0][i]][wms.ioDown[1][i]] = 1;	  // label io down
		wms.funMap[wms.ioUp[0][i]][wms.ioUp[1][i]] = 2;		  // label io up
		wms.funMap[wms.pwmDown[0][i]][wms.pwmDown[1][i]] = 3; // label pwm down
		wms.funMap[wms.pwmUp[0][i]][wms.pwmUp[1][i]] = 4;	  // label pwm up
	}
}

//------------------------ DATA HANDELING ------------------------

/// @brief Used to create @ref GateOperation::PinMapStruct (PMS) structs, which are used to
/// store information related to pin/port and wall mapping for specified functions
/// (i.e., pwm, io, up, down)
///
/// @note these methods are only used in the construtor
///
/// @param r_pms: Reference to PMS to be updated
/// @param p_port_1: Array of port values from an @ref GateOperation::WallMapStruct
/// @param p_pin_1: Array of pin values from an @ref GateOperation::WallMapStruct
void GateOperation::_makePMS(PinMapStruct &r_pms, uint8_t p_port_1[], uint8_t p_pin_1[])
{
	_resetPMS(r_pms);
	_addPortPMS(r_pms, p_port_1, p_pin_1);
	_addPinPMS(r_pms, p_port_1, p_pin_1);
}

/// @overload: Option for additional @ref GateOperation::WallMapStruct entries used
/// for creating PMS structs that include pins both up and down (e.g., all IO or all PWM pins)
///
/// @param p_port_2: Array of port values from an @ref GateOperation::WallMapStruct
/// @param p_pin_2: Array of pin values from an @ref GateOperation::WallMapStruct
void GateOperation::_makePMS(PinMapStruct &r_pms, uint8_t p_port_1[], uint8_t p_pin_1[], uint8_t p_port_2[], uint8_t p_pin_2[])
{
	_resetPMS(r_pms);
	_addPortPMS(r_pms, p_port_1, p_pin_1);
	_addPortPMS(r_pms, p_port_2, p_pin_2);
	_addPinPMS(r_pms, p_port_1, p_pin_1);
	_addPinPMS(r_pms, p_port_2, p_pin_2);
}

/// @brief Used within @ref GateOperation::_makePMS to do the actual work of adding the pin entries to the PMS structs.
///
/// @param r_pms: Reference to PMS to be updated
/// @param p_port: Array of port values from an @ref GateOperation::WallMapStruct
/// @param p_pin: Array of pin values from an @ref GateOperation::WallMapStruct
void GateOperation::_addPortPMS(PinMapStruct &r_pms, uint8_t p_port[], uint8_t p_pin[])
{

	for (size_t wal_i = 0; wal_i < 8; wal_i++)
	{ // loop port list by wall
		for (size_t prt_i = 0; prt_i < 6; prt_i++)
		{ // loop port array in struct
			if (r_pms.portInc[prt_i] != p_port[wal_i] && r_pms.portInc[prt_i] != 255)
				continue;																 // find first emtpy  or existing entry and store there
			r_pms.nPortsInc = r_pms.portInc[prt_i] == 255 ? prt_i + 1 : r_pms.nPortsInc; // update length
			r_pms.portInc[prt_i] = p_port[wal_i];
			break;
		}
	}
	// Sort array
	_sortArr(r_pms.portInc, 6);
}

/// @brief Used within @ref GateOperation::_makePMS to do the actual work of adding the port entries to the PMS structs.
///
/// @param r_pms: Reference to PMS to be updated.
/// @param p_port: Array of port values from an @ref GateOperation::WallMapStruct.
/// @param p_pin: Array of pin values from an @ref GateOperation::WallMapStruct.
void GateOperation::_addPinPMS(PinMapStruct &r_pms, uint8_t p_port[], uint8_t p_pin[])
{
	for (size_t prt_i = 0; prt_i < 6; prt_i++)
	{ // loop ports in struct arr
		if (r_pms.portInc[prt_i] == 255)
			break; // bail if reached end of list
		for (size_t wal_i = 0; wal_i < 8; wal_i++)
		{ // loop wall list
			if (p_port[wal_i] != r_pms.portInc[prt_i])
				continue; // check port match
			for (size_t pin_ii = 0; pin_ii < 8; pin_ii++)
			{ // loop pin struct
				if (r_pms.pinInc[prt_i][pin_ii] != 255)
					continue;						// find first emtpy entry and store there
				r_pms.nPinsInc[prt_i] = pin_ii + 1; // update length
				r_pms.pinInc[prt_i][pin_ii] = p_pin[wal_i];
				r_pms.wallInc[prt_i][pin_ii] = wal_i;
				break;
			}
		}
		// Sort pin and wall array based on pin number
		_sortArr(r_pms.pinInc[prt_i], 8, r_pms.wallInc[prt_i]);

		// Update registry byte
		for (size_t pin_i = 0; pin_i < r_pms.nPinsInc[prt_i]; pin_i++)
		{
			bitWrite(r_pms.byteMaskInc[prt_i], r_pms.pinInc[prt_i][pin_i], 1); // update short/truncated version
		}
		r_pms.byteMaskAll[r_pms.portInc[prt_i]] = r_pms.byteMaskInc[prt_i]; // update long/complete version
	}
}

/// @brief Sorts array values in assending order
/// Note, this is used exclusively by the above methods when creating the PMS structs.
///
/// @param p_arr: Array to be sorted
/// @param s: Length of array
/// @param p_co_arr: OPTIONAL: array to be sorted in the same order as "p_arr"
void GateOperation::_sortArr(uint8_t p_arr[], size_t s, uint8_t p_co_arr[])
{
	bool is_sorted = false;
	while (!is_sorted)
	{
		is_sorted = true;

		// Iterate over the array, swapping adjacent elements if they are out of order
		for (size_t i = 0; i < s - 1; ++i)
		{
			if (p_arr[i] > p_arr[i + 1])
			{
				// Swap the elements without using the std::swap function
				uint8_t tmp1 = p_arr[i];
				p_arr[i] = p_arr[i + 1];
				p_arr[i + 1] = tmp1;
				// Update sort ind
				if (p_co_arr)
				{
					uint8_t tmp2 = p_co_arr[i];
					p_co_arr[i] = p_co_arr[i + 1];
					p_co_arr[i + 1] = tmp2;
				}
				is_sorted = false;
			}
		}
	}
}

/// @brief Initializes/Reinitializes entries in a dynamic PMS struct to there default values.
///
/// @param r_pms: Reference to PMS struct to be reset
void GateOperation::_resetPMS(PinMapStruct &r_pms)
{
	r_pms.nPortsInc = 0;
	for (size_t prt_i = 0; prt_i < 6; prt_i++)
	{
		r_pms.portInc[prt_i] = 255;
		r_pms.nPinsInc[prt_i] = 0;
		for (size_t pin_i = 0; pin_i < 8; pin_i++)
		{
			r_pms.pinInc[prt_i][pin_i] = 255;
			r_pms.wallInc[prt_i][pin_i] = 255;
		}
		r_pms.byteMaskInc[prt_i] = 0;
		r_pms.byteMaskAll[prt_i] = 0;
	}
}

/// @brief Updates dynamic PMS structs based on new wall configuration input.
///
/// @param r_pms1: PMS struct to use as the basis for entries in "r_pms2"
/// @param r_pms2: Reference to a PMS struct to update
/// @param wall_byte_mask: Byte mask in which bits set to one denote the active walls to include in the "r_pms2" struct.
void GateOperation::_updateDynamicPMS(PinMapStruct r_pms1, PinMapStruct &r_pms2, uint8_t wall_byte_mask)
{
	for (size_t prt_i = 0; prt_i < r_pms1.nPortsInc; prt_i++)
	{ // loop ports
		for (size_t pin_i = 0; pin_i < r_pms1.nPinsInc[prt_i]; pin_i++)
		{ // loop pins
			if (bitRead(wall_byte_mask, r_pms1.wallInc[prt_i][pin_i]) == 1)
			{ // check if wall is on this port

				// Copy all struct fields
				for (size_t prt_ii = 0; prt_ii < 6; prt_ii++) // loop ports
				{

					if (r_pms2.portInc[prt_ii] != r_pms1.portInc[prt_i] && r_pms2.portInc[prt_ii] != 255)
						continue;																				// find first emtpy  or existing entry and store there
					r_pms2.nPortsInc = r_pms2.portInc[prt_ii] == 255 ? r_pms2.nPortsInc + 1 : r_pms2.nPortsInc; // update active port count for new entry
					r_pms2.nPinsInc[prt_ii]++;																	// update active pin count
					r_pms2.portInc[prt_ii] = r_pms1.portInc[prt_i];												// update active port number

					for (size_t pin_ii = 0; pin_ii < 8; pin_ii++) // loop pins
					{

						if (r_pms2.pinInc[prt_ii][pin_ii] != 255)
							continue;															 // find first emtpy entry and store there
						r_pms2.pinInc[prt_ii][pin_ii] = r_pms1.pinInc[prt_i][pin_i];			 // update active pin number
						r_pms2.wallInc[prt_ii][pin_ii] = r_pms1.wallInc[prt_i][pin_i];			 // update active wall number
						bitWrite(r_pms2.byteMaskInc[prt_ii], r_pms2.pinInc[prt_ii][pin_ii], 1);	 // update active registry byte mask
						r_pms2.byteMaskAll[r_pms2.portInc[prt_ii]] = r_pms2.byteMaskInc[prt_ii]; // update long/complete registry byte mask
						break;
					}
					break;
				}
			}
		}
	}
}

//------------------------ SETUP METHODS ------------------------

/// @brief Initialize/reset all relivant runtime variables to prepare for new session
void GateOperation::initGateOperation()
{
	_Dbg.printMsg(_Dbg.MT::HEAD1A, "START: WALL OPPERATION INITIALIZATION");

	// Update chamber address and existing walls map
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
	{
		C[cyp_i].addr = CypCom.listAddr[cyp_i];
	}

	// Reset all status tracking chamber variables
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
	{
		// Keep i2c status if reinitializing
		C[cyp_i].i2cStatus = 0;
		C[cyp_i].bitWallPosition = 0;
		C[cyp_i].bitWallMoveUpFlag = 0;
		C[cyp_i].bitWallMoveDownFlag = 0;
		C[cyp_i].bitWallErrorFlag = 0;
	}

	// Log/print initialization status
	_Dbg.printMsg(_Dbg.MT::HEAD1B, "FINISHED: WALL OPPERATION INITIALIZATION");
}

/// @brief Initialize/reset Cypress hardware
///
/// @return Output from @ref Wire::endTransmission() [0-4] or [-1=255:input argument error].
uint8_t GateOperation::initCypress()
{
	_Dbg.printMsg(_Dbg.MT::HEAD1A, "START: CYPRESS INITIALIZATION");

	// Loop through all cypress boards
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
	{
		uint8_t resp = 0;
		_Dbg.printMsg(_Dbg.MT::INFO, "INITIALIZATING: Chamber[%d] Cypress Chip[%s]", cyp_i, _Dbg.hexStr(C[cyp_i].addr));

		//............... Initialize Cypress Chip ...............

		// Setup Cypress chips and check I2C
		C[cyp_i].i2cStatus = C[cyp_i].i2cStatus > 0 ? C[cyp_i].i2cStatus : CypCom.setupCypress(C[cyp_i].addr);
		if (C[cyp_i].i2cStatus != 0)
		{
			_Dbg.printMsg(_Dbg.MT::ERROR, "Cypress Chip Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);
			continue; // skip chamber if failed
		}
		else
			_Dbg.printMsg(_Dbg.MT::INFO, "FINISHED: Cypress Chip Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);

		//............... Initialize Cypress IO ...............

		// Setup IO pins for each chamber
		C[cyp_i].i2cStatus = C[cyp_i].i2cStatus > 0 ? C[cyp_i].i2cStatus : _setupCypressIO(C[cyp_i].addr);
		if (C[cyp_i].i2cStatus != 0) // print error if failed
		{
			_Dbg.printMsg(_Dbg.MT::ERROR, "Cypress IO Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);
			continue; // skip chamber if failed
		}
		else
			_Dbg.printMsg(_Dbg.MT::INFO, "FINISHED: Cypress IO Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);

		//............... Get Starting Wall Position ...............

		/// @note: We want to get this before we setup the pwm, as this will cause the walls to move slightly
		getWallState(cyp_i, 1, C[cyp_i].bitWallPosition); // get walls in up position

		// Print warning if walls initialized in up position
		if (C[cyp_i].bitWallPosition != 0)
			_Dbg.printMsg(_Dbg.MT::WARNING, "WALLS DETECTED IN UP STATE: chamber[%d] walls%s", cyp_i, _Dbg.bitIndStr(C[cyp_i].bitWallPosition));

		//............... Initialize Cypress PWM ...............

		// Setup PWM pins for each chamber
		C[cyp_i].i2cStatus = C[cyp_i].i2cStatus > 0 ? C[cyp_i].i2cStatus : _setupCypressPWM(C[cyp_i].addr);
		if (C[cyp_i].i2cStatus != 0)
		{
			_Dbg.printMsg(_Dbg.MT::ERROR, "Cypress PWM Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);
			continue; // skip chamber if failed
		}
		else
			_Dbg.printMsg(_Dbg.MT::INFO, "FINISHED: Cypress PWM Setup: chamber=[%d|%s] status[%d]", cyp_i, _Dbg.hexStr(C[cyp_i].addr), resp);
	}

	//............... Check Status ...............

	// Set status return to any error
	uint8_t i2c_status = 0;
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
		i2c_status = i2c_status == 0 ? C[cyp_i].i2cStatus : i2c_status; // update status

	// Print status
	_Dbg.printMsg(i2c_status == 0 ? _Dbg.MT::HEAD1B : _Dbg.MT::ERROR,
				  "%s: CYPRESS INITIALIZATION: STATUS[%d]",
				  i2c_status == 0 ? "FINISHED" : "FAILED",
				  i2c_status);

	return i2c_status;
}

/// @brief Initialize/reset wall position
///
/// @param move_dir: Specify move direction [0:down, 1:up]
/// @return Success/error codes from @ref GateOperation::moveWallsByChamberBlocks()
uint8_t GateOperation::initWalls(uint8_t move_dir)
{
	uint8_t run_status = 0;
	_Dbg.printMsg(_Dbg.MT::HEAD1A, "START: WALL %s INITIALIZATION",
				  move_dir == 1 ? "UP" : "DWON");

	//............... Run Walls Up for initialize/reinitialize ...............

	if (move_dir == 1)
	{
		// Set wall movef for all cypress boards
		uint8_t byte_wall_state_new = 255; // set all walls to move up
		for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
			setWallsToMove(cyp_i, byte_wall_state_new);

		// Move walls up
		uint8_t resp = moveWallsConductor();
		run_status = run_status <= 1 ? resp : run_status; // update run status
	}

	//............... Run Walls Down for initialize/reinitialize or reset ...............

	if (move_dir == 0)
	{
		// Set wall move for all cypress boards
		uint8_t byte_wall_state_new = 0; // set all walls to move down
		for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
			setWallsToMove(cyp_i, byte_wall_state_new);

		// Move walls down
		uint8_t resp = moveWallsConductor();
		run_status = run_status <= 1 ? resp : run_status; // update run status
	}

	//............... Check Status ...............

	// Print status
	_Dbg.printMsg(run_status <= 1 ? _Dbg.MT::HEAD1B : _Dbg.MT::ERROR,
				  "%s: WALL %s INITIALIZATION: STATUS[%d]",
				  run_status <= 1 ? "FINISHED" : "FAILED",
				  move_dir == 1 ? "UP" : "DOWN", run_status);

	return run_status;
}

/// @brief Setup IO pins for each chamber including the pin direction (input) a
/// and the type of drive mode (high impedance).
///
/// @note have to do some silly stuff with the output pins as well based on
/// page 11 of the Cypress datasheet "To  allow  input  operations without
/// reconfiguration, these [output] registers have to store 1's."
///
/// @param address: I2C address of Cypress chip to setup.
/// @return method output from @ref Wire::endTransmission().
uint8_t GateOperation::_setupCypressIO(uint8_t address)
{
	uint8_t i2c_status = 0;

	// Set entire output register to off
	uint8_t p_byte_mask_in[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	i2c_status = CypCom.ioWriteReg(address, p_byte_mask_in, 6, 0);
	if (i2c_status != 0)
		return i2c_status;

	for (size_t prt_i = 0; prt_i < pmsAllIO.nPortsInc; prt_i++)
	{

		// Set input pins as input
		i2c_status = CypCom.setPortRegister(address, REG_PIN_DIR, pmsAllIO.portInc[prt_i], pmsAllIO.byteMaskInc[prt_i], 1);
		if (i2c_status != 0)
			return i2c_status;

		// Set pins as pull down
		i2c_status = CypCom.setPortRegister(address, DRIVE_PULLDOWN, pmsAllIO.portInc[prt_i], pmsAllIO.byteMaskInc[prt_i], 1);
		if (i2c_status != 0)
			return i2c_status;

		// Set corrisponding output register entries to 1 as per datasheet
		i2c_status = CypCom.ioWritePort(address, pmsAllIO.portInc[prt_i], pmsAllIO.byteMaskInc[prt_i], 1);
		if (i2c_status != 0)
			return i2c_status;
	}

	return i2c_status;
}

/// @brief Setup PWM pins for each chamber including the specifying them as PWM outputs
/// and also detting the drive mode to "Strong Drive". This also sets up the PWM
/// Source duty cycle
///
/// @param address: I2C address of Cypress chip to setup.
/// @return Wire::method output from @ref Wire::endTransmission() or [-1=255:input argument error].
uint8_t GateOperation::_setupCypressPWM(uint8_t address)
{
	uint8_t i2c_status = 0;

	// Setup PWM sources
	for (size_t src_i = 0; src_i < 8; src_i++)
	{
		i2c_status = CypCom.setupSourcePWM(address, wms.pwmSrc[src_i], pwmDuty);
		if (i2c_status != 0)
			return i2c_status;
	}

	// Setup wall pwm pins
	for (size_t prt_i = 0; prt_i < pmsAllPWM.nPortsInc; prt_i++)
	{ // loop through port list

		// Set pwm pins as pwm output
		i2c_status = CypCom.setPortRegister(address, REG_SEL_PWM_PORT_OUT, pmsAllPWM.portInc[prt_i], pmsAllPWM.byteMaskInc[prt_i], 1);
		if (i2c_status != 0)
			return i2c_status;

		// Set pins as strong drive
		i2c_status = CypCom.setPortRegister(address, DRIVE_STRONG, pmsAllPWM.portInc[prt_i], pmsAllPWM.byteMaskInc[prt_i], 1);
		if (i2c_status != 0)
			return i2c_status;
	}

	return i2c_status;
}

//------------------------ RUNTIME METHODS ------------------------

/// @brief: Method that set all walls for movement for a given chamber
///
/// @param cyp_i Index of the chamber to set [0-CypCom.nAddr].
/// @param byte_wall_state_new Byte mask with bits specifying the new wall position state [0:down, 1:up]
///
/// @return Status codes [0:no move, 1:success].@return
///
/// @details Here's an example of how to use this overload:
/// @code
/// GateOperation::WallOper.setWallsToMove(cyp_i, 0, B00000110); // move walls 2 aind 3 down
/// GateOperation::WallOper.setWallsToMove(cyp_i, 1, B00000110); // move walls 2 aind 3 up
/// @endcode
uint8_t GateOperation::setWallsToMove(uint8_t cyp_i, uint8_t byte_wall_state_new)
{

	// Bail if chamber is flagged with I2C error
	if (C[cyp_i].i2cStatus != 0)
	{
		_Dbg.printMsg(_Dbg.MT::WARNING, "SKIPPED: WALL MOVE SETUP: I2C Flagged: chamber[%d] i2c_status[%d]", cyp_i, C[cyp_i].i2cStatus);
		return 0;
	}

	// Set up/down move flags using bitwise comparison and exclude any walls with errors
	C[cyp_i].bitWallMoveUpFlag = ~C[cyp_i].bitWallErrorFlag &
								 ~C[cyp_i].bitWallPosition &
								 byte_wall_state_new;
	C[cyp_i].bitWallMoveDownFlag = ~C[cyp_i].bitWallErrorFlag &
								   C[cyp_i].bitWallPosition &
								   ~byte_wall_state_new;

	// Bail if nothing to move
	if (C[cyp_i].bitWallMoveUpFlag == 0 && C[cyp_i].bitWallMoveDownFlag == 0)
	{
		_Dbg.printMsg(_Dbg.MT::INFO, "FINISHED: WALL MOVE SETUP: No Walls to Move: chamber[%d]", cyp_i);
		return 0;
	}
	else
	{
		_Dbg.printMsg(_Dbg.MT::INFO, "FINISHED: WALL MOVE SETUP: chamber[%d] up%s down%s", cyp_i,
					  C[cyp_i].bitWallMoveUpFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveUpFlag) : "[none]",
					  C[cyp_i].bitWallMoveDownFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveDownFlag) : "[none]");
		return 1;
	}
}

/// @brief Private workhorse of the class, which mannages initiating and compleating
/// the wall movement for each block of cypress boards
///
/// @details
/// This method depends on the `bitWallRaiseFlag` variable being set by the `setWallsToMove()` method.
/// @see setWallsToMove()
///
/// @return Status/error codes [0:no move, 1:success, 2:i2c error, 3:timeout] or [-1=255:input argument error].
uint8_t GateOperation::moveWallsConductor()
{
	// Create and array of all cypress boards set to move
	uint8_t cyp_arr[CypCom.nAddr];
	uint8_t n_cyp_move = 0;

	// Find and store all cypress boards flagged for movement
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
		if (C[cyp_i].bitWallMoveUpFlag > 0 || C[cyp_i].bitWallMoveDownFlag > 0)
			cyp_arr[n_cyp_move++] = cyp_i;

	// Bail if no cypress boards set to move
	if (n_cyp_move == 0)
	{
		_Dbg.printMsg(_Dbg.MT::INFO, "SKIPPED: STAGED MOVE WALL: No Walls to Move");
		return 0;
	}

	uint8_t run_status = 0;

	// Set timeout variables
	uint32_t ts_start = millis();
	_Dbg.dtTrack(1);

	//............... Start Wall Move ...............

	for (size_t i = 0; i < n_cyp_move; i++)
	{
		size_t cyp_i = cyp_arr[i]; // get chamber index

		// Start move
		uint8_t resp = _initWallsMove(cyp_i);
		run_status = run_status <= 1 ? resp : run_status; // update overal run status

		// Print walls being moved
		_Dbg.printMsg(_Dbg.MT::INFO, "\t START: Walls Move: chamber[%d] up%s down%s error%s status[%d]",
					  cyp_i,
					  C[cyp_i].bitWallMoveUpFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveUpFlag) : "[none]",
					  C[cyp_i].bitWallMoveDownFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveDownFlag) : "[none]",
					  C[cyp_i].bitWallErrorFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallErrorFlag) : "[none]",
					  resp);
	}

	//............... Monitor Wall Move ...............

	// Initialize flags
	bool is_timedout = false;  // flag timeout
	uint8_t do_move_check = 1; // will track if all chamber movement done

	// Track wall movement
	while (!is_timedout && do_move_check != 0) // loop till finished or timed out
	{
		do_move_check = 0; // reset check flag

		for (size_t i = 0; i < n_cyp_move; i++)
		{
			size_t cyp_i = cyp_arr[i];

			// Skip if no walls still flagged to move
			if (C[cyp_i].bitWallMoveUpFlag == 0 && C[cyp_i].bitWallMoveDownFlag == 0)
				continue;

			// Check wall movement status based on IO readings
			uint8_t resp = _monitorWallsMove(cyp_i);
			run_status = run_status <= 1 ? resp : run_status; // update overal run status

			// Check for timeout
			is_timedout = millis() >= ts_start + dtMoveTimeout; // check for timeout

			// Update check flag and and timeout flag
			do_move_check += C[cyp_i].bitWallMoveUpFlag;
			do_move_check += C[cyp_i].bitWallMoveDownFlag;
		}
	}

	//............... Check/Track Final Move Status ...............

	// Check for timeout
	run_status = is_timedout ? 3 : run_status; // update overal run status

	// Check final status
	for (size_t i = 0; i < n_cyp_move; i++)
	{
		size_t cyp_i = cyp_arr[i];

		// Check status for this chamber
		bool is_err = C[cyp_i].bitWallMoveUpFlag != 0 || C[cyp_i].bitWallMoveDownFlag != 0;
		_Dbg.printMsg(is_err ? _Dbg.MT::ERROR : _Dbg.MT::INFO,
					  "%s%s: Walls Move: chamber[%d] up%s down%s error%s status[%d]",
					  is_err ? " " : "\t",
					  is_err ? "FAILED" : "FINISHED",
					  cyp_i,
					  C[cyp_i].bitWallMoveUpFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveUpFlag) : "[done]",
					  C[cyp_i].bitWallMoveDownFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallMoveDownFlag) : "[done]",
					  C[cyp_i].bitWallErrorFlag > 0 ? _Dbg.bitIndStr(C[cyp_i].bitWallErrorFlag) : "[none]",
					  run_status);

		// Handle chamber failure
		if (C[cyp_i].bitWallMoveUpFlag != 0 || C[cyp_i].bitWallMoveDownFlag != 0)
		{
			// Update error flag bitwise, set bit in error flag to 1 if it or the corresponding bit in move up or down flag is equal to 1
			C[cyp_i].bitWallErrorFlag = C[cyp_i].bitWallErrorFlag | (C[cyp_i].bitWallMoveUpFlag | C[cyp_i].bitWallMoveDownFlag);

			// Turn off all pwm for this chamber
			uint8_t resp = CypCom.ioWriteReg(C[cyp_i].addr, pmsAllPWM.byteMaskAll, 6, 0); // stop all pwm output
			run_status = run_status <= 1 ? resp : run_status;							   // update overal run status
		}
	}

	// Reset move flags
	for (size_t cyp_i = 0; cyp_i < CypCom.nAddr; cyp_i++)
	{
		C[cyp_i].bitWallMoveUpFlag = 0;
		C[cyp_i].bitWallMoveDownFlag = 0;
	}

	return run_status;
}

/// @brief Used to start wall movement through PWM output
///
/// @param cyp_i Index/number of the chamber to set [0-48]
///
/// @return Status/error codes [1:move started, 2:i2c error] or [-1=255:input argument error].
uint8_t GateOperation::_initWallsMove(uint8_t cyp_i)
{
	// Handle array inputs
	if (cyp_i > CypCom.nAddr)
		return -1;

	// Reset/Modify in dynamic PinMapStruct
	_resetPMS(C[cyp_i].pmsActvPWM);
	_resetPMS(C[cyp_i].pmsActvIO);

	// Update/Modify in dynamic PinMapStruct based on walls set to move
	_updateDynamicPMS(pmsDownIO, C[cyp_i].pmsActvIO, C[cyp_i].bitWallMoveDownFlag);	  // pwm down
	_updateDynamicPMS(pmsUpIO, C[cyp_i].pmsActvIO, C[cyp_i].bitWallMoveUpFlag);		  // pwm up
	_updateDynamicPMS(pmsDownPWM, C[cyp_i].pmsActvPWM, C[cyp_i].bitWallMoveDownFlag); // io down
	_updateDynamicPMS(pmsUpPWM, C[cyp_i].pmsActvPWM, C[cyp_i].bitWallMoveUpFlag);	  // io up

	// Move walls up/down
	uint8_t i2c_status = CypCom.ioWriteReg(C[cyp_i].addr, C[cyp_i].pmsActvPWM.byteMaskAll, 6, 1);

	// Return run status
	return i2c_status != 0 ? 2 : 1;
}

/// @brief Used to track the wall movement based on IO pins
///
/// @note both the input and output registry (up to the 6th pin) are read on each
/// call to avoid an additional read if we need to write the pwm output later
///
/// @param cyp_i Index/number of the chamber to set [0-48]
///
/// @return Status/error codes [0:still_waiting 1:all_move_down, 2:i2c error, 3:temeout] or [-1=255:input argument error].
uint8_t GateOperation::_monitorWallsMove(uint8_t cyp_i)
{
	// Handle array inputs
	if (cyp_i > CypCom.nAddr)
		return -1;

	// Local vars
	uint8_t i2c_status = 0; // track i2c status

	// Get io input and output registry bytes.
	uint8_t io_all_reg[14];
	i2c_status = CypCom.ioReadReg(C[cyp_i].addr, REG_GI0, io_all_reg, 14); // read through all input registers (6 active, 2 unused) and the 6 active output registers

	// Copy out input and output registry values seperately
	uint8_t io_in_reg[6] = {io_all_reg[0], io_all_reg[1], io_all_reg[2], io_all_reg[3], io_all_reg[4], io_all_reg[5]};		// copy out values
	uint8_t io_out_reg[6] = {io_all_reg[8], io_all_reg[9], io_all_reg[10], io_all_reg[11], io_all_reg[12], io_all_reg[13]}; // copy out values

	// Initialize output registry mask used for later if we want to turn off pwms
	uint8_t io_out_mask[6] = {0};

	// Initalize flag to track if pwm registry should be updated
	bool do_pwm_update = false;

	// Compare check ifcurrent registry io to saved registry for each port
	for (size_t prt_i = 0; prt_i < C[cyp_i].pmsActvIO.nPortsInc; prt_i++) // loop ports
	{
		/// @todo bail if no io left to track for this port based on pmsDynIO.bitMask
		uint8_t port_n = C[cyp_i].pmsActvIO.portInc[prt_i];

		// Compare current io registry to the active io pin byte mask
		/// @note: Triggered pin/bit matching mask will be 1
		uint8_t io_change_check_byte = C[cyp_i].pmsActvIO.byteMaskInc[prt_i] & io_in_reg[port_n];

		// Check each bit in comparison byte
		for (size_t pin_i = 0; pin_i < C[cyp_i].pmsActvIO.nPinsInc[prt_i]; pin_i++) // loop pins
		{
			uint8_t pin_n = C[cyp_i].pmsActvIO.pinInc[prt_i][pin_i];

			// Skip if pin no longer flagged in reg byte
			if (!bitRead(io_change_check_byte, pin_n))
				continue;

			// Modify in dynamic PinMapStruct to remove pin from bit mask
			bitWrite(C[cyp_i].pmsActvIO.byteMaskInc[prt_i], pin_n, 0); // remove wall/pin from byte reg

			// Update pwm registry array
			/// @note: sets both up and down pwm reg entries to be turned off as this makes the code easier
			uint8_t wall_n = C[cyp_i].pmsActvIO.wallInc[prt_i][pin_i]; // get wall number
			bitWrite(io_out_mask[wms.pwmDown[0][wall_n]], wms.pwmDown[1][wall_n], 1);
			bitWrite(io_out_mask[wms.pwmUp[0][wall_n]], wms.pwmUp[1][wall_n], 1);

			// Get triggered switch [1:io_down, 2:io_up]
			uint8_t swtch_fun = wms.funMap[port_n][pin_n];

			// Update state [0,1] [down,up] based on the triggered switch
			bitWrite(C[cyp_i].bitWallPosition, wall_n, swtch_fun == 1 ? 0 : 1);

			// Set both flags to false for convenience
			bitWrite(C[cyp_i].bitWallMoveUpFlag, wall_n, 0);   // reset wall bit in flag
			bitWrite(C[cyp_i].bitWallMoveDownFlag, wall_n, 0); // reset wall bit in flag

			/// Unset error flag
			/// @todo: Consider if error flag should be reset here
			bitWrite(C[cyp_i].bitWallErrorFlag, wall_n, 0);

			// Flag to update pwm
			do_pwm_update = true;

			// Print wall move finished message
			_Dbg.printMsg(_Dbg.MT::INFO, "\t\t FINISHED: Wall Move: chamber[%d] wall[%d][%s] dt[%s]",
						  cyp_i, wall_n, swtch_fun == 1 ? "down" : "up", _Dbg.dtTrack());
		}
	}

	// Send pwm off command if move complete or timed out
	if (do_pwm_update)
	{																					 // check for update flag
		uint8_t resp = CypCom.ioWriteReg(C[cyp_i].addr, io_out_mask, 6, 0, io_out_reg); // include last reg read and turn off pwms
		i2c_status = i2c_status == 0 ? resp : i2c_status;								 // update i2c status
	}

	// Return run status
	return i2c_status != 0 ? 2 : (C[cyp_i].bitWallMoveUpFlag == 0 && C[cyp_i].bitWallMoveDownFlag == 0);
}

/// @brief Used to get the current wall state/position based on limit switch IO
///
/// @param cyp_i Index/number of the chamber to set [0-CypCom.nAddr].
/// @param pos_state_get Value specifying the wall position switch to get [0:down, 1:up].
/// @param byte_state_out Byte reference  to store state of up or down switches by wall (used as output).
///
/// @return Wire::method output [0-4] or [-1=255: input argument error]
uint8_t GateOperation::getWallState(uint8_t cyp_i, uint8_t pos_state_get, uint8_t &byte_state_out)
{
	if (cyp_i > CypCom.nAddr)
		return -1;

	// Get io input registry bytes.
	uint8_t io_in_reg[6];
	uint8_t resp = CypCom.ioReadReg(C[cyp_i].addr, REG_GI0, io_in_reg, 6);
	if (resp != 0)
		return resp;

	// Get wall io state
	for (size_t wall_i = 0; wall_i < 8; wall_i++)
	{
		// Get down io state
		if (pos_state_get == 0)
			bitWrite(byte_state_out, wall_i, bitRead(io_in_reg[wms.ioDown[0][wall_i]], wms.ioDown[1][wall_i]));
		// Get up io state
		else
			bitWrite(byte_state_out, wall_i, bitRead(io_in_reg[wms.ioUp[0][wall_i]], wms.ioUp[1][wall_i]));
	}

	return resp;
}

//------------------------ TESTING AND DEBUGGING METHODS ------------------------

/// @brief Used for testing the limit switch IO pins on a given Cypress chip.
/// @details This will loop indefinitely. Best to put this in the Arduino Setup() function.
///
/// @param cyp_i Index/number of the chamber to set [0-48]
/// @param p_wall_inc OPTIONAL: Pointer array specifying wall index/number for wall(s) to test [0-7], max 8 entries. DEFAULT: all walls
/// @param s OPTIONAL: Length of "p_wall_inc" array. DEFAULT: 8
///
/// @return Wire::method output [0-4] or [-1=255: input argument error].
///
/// @example Here's an example of how to use testWallIO
/// uint8_t cyp_i = 0; // Index of the chamber
/// uint8_t s = 3; // Number of walls
/// uint8_t p_wall_inc[s] = {0, 2, 5}; // Array with wall numbers to move
/// GateOperation::testWallIO(cyp_i, a_wall, s); // This can be run more than once to setup multiple cypress boards
/// GateOperation::testWallIO(0); // This will test all walls in chamber 0
uint8_t GateOperation::testWallIO(uint8_t cyp_i, uint8_t p_wall_inc[], uint8_t s)
{
	if (cyp_i > CypCom.nAddr || s > 8)
		return -1;

	// initialize array to handle null array argument
	uint8_t p_wi[s];
	if (p_wall_inc == nullptr)
	{ // set default 8 walls
		for (size_t i = 0; i < s; i++)
			p_wi[i] = i;
	}
	else
	{ // copy over input
		for (size_t i = 0; i < s; i++)
			p_wi[i] = p_wall_inc[i];
	}

	// Test input pins
	uint8_t r_bit_out;
	uint8_t resp = 0;
	_Dbg.printMsg(_Dbg.MT::HEAD1, "RUNNING: Test IO switches: chamber[%d] walls%s", cyp_i, _Dbg.arrayStr(p_wi, s));
	while (true)
	{ // loop indefinitely
		for (size_t i = 0; i < s; i++)
		{ // loop walls
			uint8_t wall_n = p_wi[i];

			// Check down pins
			resp = CypCom.ioReadPin(C[cyp_i].addr, wms.ioDown[0][wall_n], wms.ioDown[1][wall_n], r_bit_out);
			if (resp != 0) // break out of loop if error returned
				break;
			if (r_bit_out == 1)
				_Dbg.printMsg(_Dbg.MT::INFO, "\t Wall %d: down", wall_n);

			// Check up pins
			resp = CypCom.ioReadPin(C[cyp_i].addr, wms.ioUp[0][wall_n], wms.ioUp[1][wall_n], r_bit_out);
			if (resp != 0) // break out of loop if error returned
				break;
			if (r_bit_out == 1)
				_Dbg.printMsg(_Dbg.MT::INFO, "\t Wall %d: up", wall_n);

			// Add small delay
			delay(10);
		}
	}
	// Print failure message if while loop is broken out of because of I2C coms issues
	_Dbg.printMsg(_Dbg.MT::ERROR, "Test IO switches: chamber[%d] walls%s", cyp_i, _Dbg.arrayStr(p_wi, s));
	return resp;
}

/// @brief Used for testing the motor PWM outputs for a given Cypress chip.
/// @details Note, best to put this in the Arduino Setup() function.
///
/// @param cyp_i Index/number of the chamber to set [0-48]
/// @param p_wall_inc OPTIONAL: Pointer array specifying wall index/number for wall(s) to test [0-7], max 8 entries. DEFAULT: all walls
/// @param s OPTIONAL: Length of "p_wall_inc" array. DEFAULT: 8
///
/// @return Wire::method output [0-4] or [-1=255: input argument error].
///
/// @see GateOperation::testWallIO()
/// @example Refer to the example provided in @ref GateOperation::testWallIO().
uint8_t GateOperation::testWallPWM(uint8_t cyp_i, uint8_t p_wall_inc[], uint8_t s, uint16_t dt_run)
{
	if (cyp_i > CypCom.nAddr || s > 8)
		return -1;
	uint8_t p_wi[s];
	if (p_wall_inc == nullptr)
	{ // set default 8 walls
		for (size_t i = 0; i < s; i++)
			p_wi[i] = i;
	}
	else
	{ // copy over input
		for (size_t i = 0; i < s; i++)
			p_wi[i] = p_wall_inc[i];
	}

	// Run each wall up then down for dt_run ms
	_Dbg.printMsg(_Dbg.MT::HEAD1, "RUNNING: Test PWM: chamber[%d] walls%s", cyp_i, _Dbg.arrayStr(p_wi, s));
	uint8_t resp = 0;
	for (size_t i = 0; i < s; i++)
	{ // loop walls
		uint8_t wall_n = p_wi[i];
		_Dbg.printMsg(_Dbg.MT::INFO, "\t Wall %d: Up", wall_n);
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmUp[0][wall_n], wms.pwmUp[1][wall_n], 1); // run wall up
		if (resp != 0)
			return resp;
		delay(dt_run);
		_Dbg.printMsg(_Dbg.MT::INFO, "\t Wall %d: Down", wall_n);
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmDown[0][wall_n], wms.pwmDown[1][wall_n], 1); // run wall down (run before so motoro hard stops)
		if (resp != 0)
			return resp;
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmUp[0][wall_n], wms.pwmUp[1][wall_n], 0); // stop wall up pwm
		if (resp != 0)
			return resp;
		delay(dt_run);
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmDown[0][wall_n], wms.pwmDown[1][wall_n], 0); // stop wall down pwm
		if (resp != 0)
			return resp;
	}
	return resp;
}

/// @brief Used for testing the overall wall module function for a given Cypress chip.
/// @details Note, best to put this in the Arduino Setup() function.
///
/// @param cyp_i Index/number of the chamber to set [0-48]
/// @param p_wall_inc OPTIONAL: Pointer array specifying wall index/number for wall(s) to test [0-7], max 8 entries. DEFAULT: all walls
/// @param s OPTIONAL: Length of "p_wall_inc" array. DEFAULT: 8
///
/// @return Wire::method output [0-4] or [-1=255: input argument error].
///
/// @see GateOperation::testWallIO()
/// @example Refer to the example provided in @ref GateOperation::testWallIO().
uint8_t GateOperation::testWallOperation(uint8_t cyp_i, uint8_t p_wall_inc[], uint8_t s)
{
	if (cyp_i > CypCom.nAddr || s > 8)
		return -1;
	uint8_t p_wi[s];
	if (p_wall_inc == nullptr)
	{ // set default 8 walls
		for (size_t i = 0; i < s; i++)
			p_wi[i] = i;
	}
	else
	{ // copy over input
		for (size_t i = 0; i < s; i++)
			p_wi[i] = p_wall_inc[i];
	}

	// Test all walls
	_Dbg.printMsg(_Dbg.MT::HEAD1, "RUNNING: Test move operation: chamber[%d] walls%s", cyp_i, _Dbg.arrayStr(p_wi, s));
	uint8_t r_bit_out = 1;
	uint16_t dt = 2000;
	uint16_t ts;
	uint8_t resp = 0;
	for (size_t i = 0; i < s; i++)
	{ // loop walls
		uint8_t wall_n = p_wi[i];
		_Dbg.printMsg(_Dbg.MT::INFO, "\t Moving wall %d", wall_n);

		// Run up
		_Dbg.printMsg(_Dbg.MT::INFO, "\t\t up start");
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmUp[0][wall_n], wms.pwmUp[1][wall_n], 1);
		if (resp != 0)
			return resp;
		ts = millis() + dt; // set timeout
		_Dbg.dtTrack(1);	// start timer
		while (true)
		{ // check up switch
			resp = CypCom.ioReadPin(C[cyp_i].addr, wms.ioUp[0][wall_n], wms.ioUp[1][wall_n], r_bit_out);
			if (resp != 0)
				return resp;
			if (r_bit_out == 1)
			{
				_Dbg.printMsg(_Dbg.MT::INFO, "\t\t up end [%s]", _Dbg.dtTrack());
				break;
			}
			else if (millis() >= ts)
			{
				_Dbg.printMsg(_Dbg.MT::INFO, "\t\t !!up timedout [%s]", _Dbg.dtTrack());
				break;
			}
			delay(10);
		}

		// Run down
		_Dbg.printMsg(_Dbg.MT::INFO, "\t\t down start");
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmDown[0][wall_n], wms.pwmDown[1][wall_n], 1);
		if (resp != 0)
			return resp;
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmUp[0][wall_n], wms.pwmUp[1][wall_n], 0);
		if (resp != 0)
			return resp;
		ts = millis() + dt; // set timeout
		_Dbg.dtTrack(1);	// start timer
		while (true)
		{ // check up switch
			resp = CypCom.ioReadPin(C[cyp_i].addr, wms.ioDown[0][wall_n], wms.ioDown[1][wall_n], r_bit_out);
			if (resp != 0)
				return resp;
			if (r_bit_out == 1)
			{
				_Dbg.printMsg(_Dbg.MT::INFO, "\t\t down end [%s]", _Dbg.dtTrack());
				break;
			}
			else if (millis() >= ts)
			{
				_Dbg.printMsg(_Dbg.MT::INFO, "\t\t !!down timedout [%s]", _Dbg.dtTrack());
				break;
			}
			delay(10);
		}
		resp = CypCom.ioWritePin(C[cyp_i].addr, wms.pwmDown[0][wall_n], wms.pwmDown[1][wall_n], 0);
		if (resp != 0)
			return resp;

		// Pause
		delay(500);
	}
	return resp;
}

/// @brief Used for debugging to print out all fields of a PMS struct.
///
/// @param p_wall_inc: OPTIONAL: [0-7] max 8 entries. DEFAULT: all walls
/// @param s: OPTIONAL: length of @param p_wall_inc array. DEFAULT: 8
void GateOperation::_printPMS(PinMapStruct pms)
{
	_Dbg.printMsg(_Dbg.MT::DEBUG, "IO/PWM nPorts[%d]_____________________", pms.nPortsInc);
	for (size_t prt_i = 0; prt_i < pms.nPortsInc; prt_i++)
	{
		_Dbg.printMsg(_Dbg.MT::DEBUG, "port[%d] nPins[%d] bitMask[%s]", pms.portInc[prt_i], pms.nPinsInc[prt_i], _Dbg.binStr(pms.byteMaskInc[prt_i]));
		for (size_t pin_i = 0; pin_i < pms.nPinsInc[prt_i]; pin_i++)
		{
			_Dbg.printMsg(_Dbg.MT::DEBUG, "\t wall[%d] pin[%d]", pms.wallInc[prt_i][pin_i], pms.pinInc[prt_i][pin_i]);
		}
	}
}