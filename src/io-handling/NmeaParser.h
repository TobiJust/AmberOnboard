/*
 * NmeaParser.h
 *
 *  Created on: 21.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef NMEAPARSER_H_
#define NMEAPARSER_H_

#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef enum {
    PARSE_OK,
    PARSE_ERR_INVALID,
    PARSE_ERR_INCOMPLETE
}parseState;

typedef struct gga {
    string utc;
    string latVal;
    string latNS;
    string longVal;
    string longEW;
    string quality;
    string satCnt;
    string hdop;
    string height;
    string hUnit;
    string sepVal;
    string sepUnit;
    string dgpsAge;
    string dgpsRef;
}gga;

class NmeaParser {
public:

    static uint8_t parseGGA(string source, gga &dataset);

private:

    static void tokenize(string source, vector<string> &target, char delim);

    NmeaParser();
    virtual ~NmeaParser();
};

#endif /* NMEAPARSER_H_ */
