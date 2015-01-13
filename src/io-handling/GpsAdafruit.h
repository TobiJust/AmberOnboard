/*
 * GpsAdafruit.h
 *
 *  Created on: 21.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef GPSADAFRUIT_H_
#define GPSADAFRUIT_H_

#include "instances/IOserial.h"
#include "Device.h"
#include "NmeaParser.h"

#include <mutex>
#include <sstream>
#include <string>

#include <cstdint>

// Message id macros.
#define PMTK_SET_NMEA_UPDATERATE    220
#define PMTK_SET_NMEA_BAUDRATE      251
#define PMTK_API_SET_NMEA_OUTPUT    314
#define PMTK_API_Q_NMEA_OUTPUT      414
#define PMTK_API_DT_NMEA_OUTPUT     514

// NMEA sentence macros.
#define NMEA_SEN_DISABLE     0
#define NMEA_SEN_EVERY       1
#define NMEA_SEN_EVERY_TWO   2
#define NMEA_SEN_EVERY_THREE 3
#define NMEA_SEN_EVERY_FOUR  4
#define NMEA_SEN_EVERY_FIVE  5


typedef enum {
    AF_OK,
    AF_ERR_OPEN,
    AF_ERR_INVALID_BAUD,
    AF_ERR_INVALID_NMEA_UPDATE,
    AF_ERR_INVALID_FIX_INTERVAL
}adafruitState;

using namespace std;

class GpsAdafruit : public Device {
public:

    GpsAdafruit(string path, uint32_t baud);
    virtual ~GpsAdafruit();

    virtual int run();
    virtual void getValues(unordered_map<string, string> &values);

    uint8_t initialize();

    uint8_t setUpdate(uint16_t interval);
    uint8_t setBaud(uint32_t baud);
    uint8_t setNMEA(
            uint8_t gll,
            uint8_t rmc,
            uint8_t vtg,
            uint8_t gga,
            uint8_t gsa,
            uint8_t gsv);

    // uint8_t scanNMEA();
    // uint8_t scanGPGGA();

    uint8_t printFrame(uint16_t msgType, string content);
    uint8_t getChecksum(string source);

private:

    // Serial port access.
    IOserial device;

    // Last known data.
    unordered_map<string, string> values;

    // Data access mutex.
    mutex rw_mutex;

};

#endif /* GPSADAFRUIT_H_ */
