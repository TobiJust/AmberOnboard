/*
 * Config.h
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define COMMENT '#'
#define KEY_VAL_DELIM '='
#define VAL_VAL_DELIM ','
#define DEV_PATH "/dev/"

#define EQUALS(SRC, POS, TARG)   (!SRC.compare(POS, sizeof(TARG)-1, TARG))

#define OPT_OBU_ID      "obu-id"
#define OPT_REAL_ADDR   "rt-addr"
#define OPT_REAL_IFACE  "rt-iface"
#define OPT_DEF_ADDR    "def-addr"
#define OPT_DEF_IFACE   "def-iface"
#define OPT_CAP_OUT     "cap-out"
#define OPT_CAP_IN      "cap-in"
#define OPT_CAP_PRIME   "cap-prime"
#define OPT_CAP_COMP    "cap-comp"
#define OPT_GPS_DEV     "gps-dev"
#define OPT_GPS_TYPE    "gps-type"
#define OPT_ACC_DEV     "acc-dev"
#define OPT_ACC_TYPE    "acc-type"
#define OPT_OBD_DEV     "obd-dev"

#define GPS_ADAFRUIT    "adafruit"

#define ACC_MPU6050     "mpu6050"

#include "instances/IOfile.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cstdint>
#include <cstring>

typedef enum {
    CONF_OK,
    CONF_ERR_NO_SUCH_FILE,
    CONF_ERR_READ,
    CONF_ERR_UNKNOWN_OPT,
    CONF_ERR_ALREADY_SET,
    CONF_ERR_TUPLE,
    CONF_ERR_COUNT_MISMATCH,
    CONF_ERR_INVALID,
    CONF_ERR_UNSET
}conf_return;

typedef enum {
    T_ADAFRUIT
}gpsTypes;

typedef enum {
    T_MPU6050
}accTypes;

typedef struct terminal {
    string path;
    uint32_t baud;
}terminal;

typedef struct server {
    string target;
    string port;
    string iface;
}server;

typedef struct capture {
    uint8_t index;
    uint8_t fps;
}capture;

typedef struct i2cDev {
    string path;
    uint8_t addr;
}i2cDev;

typedef struct uartDev {
    string path;
    uint32_t baud;
}uartDev;

using namespace std;

class Config {
public:

    Config(string path);
    virtual ~Config();

    uint8_t load();
    // uint8_t save();

    // Getters.
    vector<string> getErrors();
    bool getDeviceID(uint8_t &devID);
    uint8_t getTermCount();
    bool getTermAt(uint8_t index, terminal &term);
    bool getRealTime(server &serv);
    bool getDeferred(server &serv);
    bool getInnerCap(capture &cap);
    bool getOuterCap(capture &cap);
    bool getPrimeCap(uint8_t &index);
    bool getJpegCompression(uint8_t &comp);
    bool getAccType(uint8_t &type);
    bool getAcc(i2cDev &dev);
    bool getGPSType(uint8_t &type);
    bool getGPS(uartDev &dev);
    bool getOBD(uartDev &dev);

    static void tokenize(string source, vector<string> &target, char delim);
    static bool shrink(string &target, string compare, size_t start, bool substr=false);
    static bool toInteger(string source, int64_t max, int64_t min, int64_t &target);

private:

    IOfile file;

    // List of parsing errors.
    vector<string> errors;

    // List of already parsed options.
    unordered_set<string> parsed;

    // OBU device id.
    uint8_t devID;

    // Realtime / deferred server properties.
    server realtime, deferred;

    // Capture structures and primary capture index.
    capture outer, inner;
    uint8_t capPrimary;

    // JPEG compression quality.
    uint8_t comp;

    // Accelerometer (typically i2c)
    uint8_t accType;
    i2cDev acc;

    // GPS sensor (typically uart)
    uint8_t gpsType;
    uartDev gps;

    // OBD module.
    uartDev obd;

    // Parsing helper methods.
    static vector<string> getLines(string source);
    static bool toKeyValue(string source, vector<string> &target);
    static void toAbsPath(string source, string &target);
    static string toString(string option, uint8_t status);

    // Process functions.
    void process(string source);
    bool validate();
    static uint8_t procID(vector<string> source, uint8_t &devId);
    static uint8_t procTerm(vector<string> source, terminal &term);
    static uint8_t procSvr(vector<string> source, server &server);
    static uint8_t procIface(vector<string> source, server &server);
    static uint8_t procCapture(vector<string> source, capture &capture);
    static uint8_t procPrimary(vector<string> source, uint8_t &prime);
    static uint8_t procCompression(vector<string> source, uint8_t &comp);
    static uint8_t procAcc(vector<string> source, i2cDev &acc);
    static uint8_t procAccType(vector<string> source, uint8_t &accType);
    static uint8_t procGPS(vector<string> source, uartDev &gps);
    static uint8_t procGPSType(vector<string> source, uint8_t &gpsType);
    static uint8_t procOBD(vector<string> source, uartDev &obd);

};

#endif /* CONFIG_H_ */
