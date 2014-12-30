/*
 * NmeaParser.cpp
 *
 *  Created on: 21.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "NmeaParser.h"

#include <iostream>

NmeaParser::NmeaParser() { }

NmeaParser::~NmeaParser() { }

uint8_t NmeaParser::parseGGA(string source, gga &dataset) {

    if (source.compare(0, 6, "$GPGGA"))
        return PARSE_ERR_INVALID;

    // tokenize per ','
    vector<string> token;
    tokenize(source, token, ',');

    if (token.size() < 15)
        return PARSE_ERR_INCOMPLETE;

    dataset.utc      = token[1];
    dataset.latVal   = token[2]+token[3];
    dataset.longVal  = token[4]+token[5];
    dataset.quality  = token[6];
    dataset.satCnt   = token[7];
    dataset.hdop     = token[8];
    dataset.height   = (token[9].size()? token[9]+token[10] : "" );
    dataset.sepVal   = token[11]+token[12];
    dataset.dgpsAge  = token[13];

    // Tokenize last data string (delimited by '*').
    tokenize(token[14], token, '*');

    dataset.dgpsRef  = token[token.size()-2];

    cerr << "UTC:   " << dataset.utc << endl;
    cerr << "Lat.:  " << dataset.latVal << endl;
    cerr << "Long.: " << dataset.longVal << endl;
    cerr << "Qual.: " << dataset.quality << endl;
    cerr << "Sat.:  " << dataset.satCnt << endl;
    cerr << "HGOP.: " << dataset.hdop << endl;
    cerr << "hght.: " << dataset.height << endl;
    cerr << "sep.:  " << dataset.sepVal << endl;
    cerr << "d-age: " << dataset.dgpsAge << endl;
    cerr << "d-ref: " << dataset.dgpsRef << endl;

    return PARSE_OK;
}

void NmeaParser::tokenize(string source, vector<string> &target, char delim) {

    istringstream stream(source);
    for(string token; getline(stream, token, delim); )
        target.push_back(token);
}
