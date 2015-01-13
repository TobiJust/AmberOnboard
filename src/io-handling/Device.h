/*
 * Device.h
 *
 *  Created on: 04.01.2015
 *      Author: Daniel Wagenknecht
 */

#ifndef DEVICE_H_
#define DEVICE_H_

// Keys for result list.
#define GPS_POS_LAT    "longitude"
#define GPS_POS_LONG   "latitude"
#define GPS_POS_HEIGHT "height"

#define ACC_X   "acc x"
#define ACC_Y   "acc y"
#define ACC_Z   "acc z"
#define GYRO_X   "gyro x"
#define GYRO_Y   "gyro y"
#define GYRO_Z   "gyro z"

#define OBD_SPEED       "speed"
#define OBD_RPM         "rpm"
#define OBD_ENG_LOAD    "engine load"
#define OBD_COOL_TEMP   "coolant temp"
#define OBD_AIR_FLOW    "air flow"
#define OBD_INLET_PRESS "inlet pressure"
#define OBD_INLET_TEMP  "inlet temp"
#define OBD_FUEL_LVL    "fuel level"
#define OBD_FUEL_PRESS  "fuel pressure"
#define OBD_ENG_KM      "distance"

#include <mutex>
#include <string>
#include <unordered_map>

using namespace std;

class Device {
public:

    Device();
    virtual ~Device();

    virtual int run()=0;
    void terminate();
    bool isTerminating();

    virtual void getValues(unordered_map<string, string> &values)=0;

protected:

    static void addValue(unordered_map<string, string> &values, string key, string value);

private:

    bool terminating;
};

#endif /* DEVICE_H_ */
