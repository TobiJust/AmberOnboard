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
#define VAL_VAL_DELIM ':'

#define EQUALS(SRC, POS, TARG)   SRC.compare(POS, sizeof(TARG)-1, TARG)

#define OPT_TERMINAL    "terminal"
#define OPT_SERV_RT     "serv-rt"
#define OPT_SERV_NONRT  "serv-nonrt"
#define OPT_CAPTURES    "captures"
#define OPT_GPS         "gps"
#define OPT_ACC         "acc"
#define OPT_LOG_PREFIX  "log"
#define OPT_STATUS      "-status"
#define OPT_FAIL        "-failure"
#define OPT_POS         "-position"
#define OPT_VALOCITY    "-velocity"
#define OPT_EVENT       "-event"
#define OPT_UNKNOWN     "unknown"

#define GPS_ADAFRUIT    "adafruit"

#define ACC_MPU6050     "mpu6050"

#include "instances/IOfile.h"
#include "../Child.h"

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cstdint>

typedef enum {
    CONF_OK,
    CONF_ERR_IO,
    CONF_ERR_NO_VALUE
}confState;

typedef enum {
    GPS_TYPE_ADAFRUIT
}gpsTypes;

typedef enum {
    ACC_TYPE_MPU6050
}accTypes;

typedef struct terminal {
    string path;
    uint32_t baud;
    bool status;
    bool error;
    bool pos;
    bool vel;
    bool event;
}terminal;

typedef struct server {
    string address;
    string port;
    string iface;
}server;

typedef struct captures {
    uint16_t outer;
    uint16_t inner;
    uint16_t primary;
}captures;

typedef struct gps {
    string path;
    uint32_t baud;
    uint8_t type;
}gpsModule;

typedef struct acc {
    string path;
    uint32_t addr;
    uint8_t type;
}accelerometer;

using namespace std;

class Config : public Child {
public:

    Config(string path);
    virtual ~Config();

    virtual int run();

    uint8_t load();
    uint8_t save();

    uint8_t getTermCount();
    bool getTermAt(uint8_t index, terminal &term);
    void getServRT(server &serv);
    void getServNonRT(server &serv);
    int16_t getOuterCap();
    int16_t getInnerCap();
    int16_t getPrimaryCap();

    void getGPS(gps &module);
    void getAcc(acc &module);

private:

    IOfile file;

    unordered_map<string, string> optErr;
    unordered_map<string, string> config;

    vector<terminal> terminals;
    server realtime, non_realtime;
    captures caps;
    gps gps_Module;
    acc acc_Module;

    static void tokenize(string source, vector<string> &target, char delim);
    static void prepare(string source, vector<string> &target);
    uint8_t process(vector<string> &source /*, unordered_map<string, string> &optErr*/);

    static bool procTerm(string key, string value, terminal &term, stringstream &failures);
    static bool procServ(string key, string value, server &serv, stringstream &failures);
    static bool procCaps(string key, string value, captures &caps, stringstream &failures);
    static bool procGPS(string key, string value, gps &gps, stringstream &failures);
    static bool procAcc(string key, string value, acc &acc, stringstream &failures);


    static void insertOrAppend(unordered_map<string, string> &optErr, string key, stringstream &value);
    static void printTermNote(stringstream &stream);
    static void printServNote(stringstream &stream);
    static void printCapsNote(stringstream &stream);
    static void printGPSNote(stringstream &stream);
    static void printAccNote(stringstream &stream);

};

#endif /* CONFIG_H_ */
