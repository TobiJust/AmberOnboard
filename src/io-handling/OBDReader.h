/**
 * \brief Header file for OBDReader class
 *
 * \detail defines the function for initializing the OBD-Interface and getting the car data from it.
 *
 * \class ODBReader
 *
 * \version 2014-12-27
 * \author Felix Hahn
 */

#ifndef SRC_OBDREADER_H_
#define SRC_OBDREADER_H_
//OBD-Commands for interface initializing
#define OBD_RESET "ATZ\r"
#define OBD_ECHO_ON "ATE1\r"
#define OBD_ECHO_OFF "ATE0\r"
#define OBD_HEADER_ON "ATH1\r"
#define OBD_HEADER_OFF "ATH0\r"
#define OBD_BLANKS_ON "ATS1\r"
#define OBD_BLANKS_OFF "ATS0\r"
#define OBD_LINEFEED_OFF "ATL0\r"
//line delimiter for serial communication
#define OBD_DELIM 0xa
//answer string from OBD-Interface after a reset
#define OBD_ATZ_ANSWER "ELM327 v1.5\n"

#define SLEEP_DURATION 1000

#include "instances/IOserial.h"
#include "Device.h"

#include <mutex>
#include <string>
#include <cstdint>

using namespace std;


class OBDReader : public Device {
public:
	//constructor
	OBDReader(string path, uint32_t baud);

	//deconstructor
	virtual ~OBDReader();

    virtual int run();
    virtual void getValues(unordered_map<string, string> &values);

	//functions for setting up the adapter
	uint8_t reset();
	uint8_t setup();

	//function for getting data from adapter
	int16_t getAdapter();
	int16_t getSpeed();
	int16_t getRPM();
	int16_t getEngineLoad();
	int16_t getCoolantTemp();
	int16_t getAirpressureInlet();
	int16_t getInletAirTemp();
	int16_t getAirflow();
	int16_t getEngineControlKM();
	int32_t getFuelPressure();
	int16_t getFuelLevel();

	//function for parsing the data from the adapter
	uint8_t getOBDAnswer(string toCompare, string &input);

private:

	IOserial device;

    // Last known data.
    unordered_map<string, string> values;

    // Data access mutex.
    mutex rw_mutex;

};


#endif /* SRC_OBDREADER_H_ */
