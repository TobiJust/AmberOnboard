/*
 * GpsAdafruit.cpp
 *
 *  Created on: 21.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "GpsAdafruit.h"

#include <iostream>

GpsAdafruit::GpsAdafruit(string path, uint32_t baud) : device(path, baud) { }

GpsAdafruit::~GpsAdafruit() {
    this->device.close();
}

uint8_t GpsAdafruit::initialize() {

    uint8_t status = AF_OK;

    // Force device open.
    status = this->device.open(OPEN_W, true);
    if (status) return AF_ERR_OPEN;

    // Target baud rate to set.
    uint32_t targetBaud = this->device.getBaud();

    // Iterate possible baud rates to set target baud rate.
    for ( uint32_t baud : {4800, 9600, 14400, 19200, 38400, 57600, 115200} ) {
        status = (this->setBaud(baud));
        status += this->setBaud(targetBaud);

        if (status) return status;
    }

    // Set NMEA output string.
    status = this->setNMEA(0,0,0,1,0,0);
    if (status) return status;

    // Set update rate.
    status = this->setUpdate(100);

    return status;
}

uint8_t GpsAdafruit::printFrame(uint16_t msgType, string content) {

    // TODO: delete that!
    this->device.close();

    // Build checksum-relevant content and calculate checksum.
    stringstream checksumStream;
    checksumStream << "PMTK" << msgType << (content.length()>0?",":"") << content;
    uint8_t checksum = this->getChecksum(checksumStream.str());

    // Get checksums HIGH and LOW byte.
    stringstream frame;
    frame << "$" << checksumStream.str() << '*';
    frame << uppercase << hex << (uint16_t)checksum;
    frame << "\r\n";

    // this->device.appendOpen(frame.str());

    cerr << frame.str() << endl;

    return AF_OK;
}

uint8_t GpsAdafruit::getChecksum(string source) {

    // Result checksum.
    uint8_t result=0;

    // Iterate over string and calculate XOR-checksum.
    for (uint32_t index = 0; index < source.length(); index++)
        result ^= source[index];

    return result;

}

uint8_t GpsAdafruit::setBaud(uint32_t baud) {

    // Check if baud rate is supported by device.
    if (    baud!=  4800 &&
            baud!=  9600 &&
            baud!= 14400 &&
            baud!= 19200 &&
            baud!= 38400 &&
            baud!= 57600 &&
            baud!=115200 )
        return AF_ERR_INVALID_BAUD;

    // Build data frame and write it to terminal.
    uint8_t status = printFrame(PMTK_SET_NMEA_BAUDRATE, to_string(baud));

    this->device.setBaud(baud);

    string result;

    for (int i=0; i<25; i++) {
        (int)this->device.readOpen(result,'\n');
        cerr << result.size() << ": " << result << endl;
    }

    return status;

}

uint8_t GpsAdafruit::setNMEA(
        uint8_t gll,
        uint8_t rmc,
        uint8_t vtg,
        uint8_t gga,
        uint8_t gsa,
        uint8_t gsv) {

    // Check if update rates are valid.
    if (    gll > 5 ||
            rmc > 5 ||
            vtg > 5 ||
            gga > 5 ||
            gsa > 5 ||
            gsv > 5 )
        return AF_ERR_INVALID_NMEA_UPDATE;

    stringstream content;

    // Set supported NMEA sentences.
    content << (uint8_t)('0' + gll) << ",";
    content << (uint8_t)('0' + rmc) << ",";
    content << (uint8_t)('0' + vtg) << ",";
    content << (uint8_t)('0' + gga) << ",";
    content << (uint8_t)('0' + gsa) << ",";
    content << (uint8_t)('0' + gsv) << ",";

    // Set reserved NMEA sentences.
    content << "0,0,0,0,0,0,0,0,0,0,0,0,0";

    // Build data frame and write it to terminal.
    return this->printFrame(PMTK_API_SET_NMEA_OUTPUT, content.str());
}

uint8_t GpsAdafruit::setUpdate(uint16_t interval) {

    // Check if fix interval is valid.
    if (interval < 100 || interval > 10000)
        return AF_ERR_INVALID_FIX_INTERVAL;

    return this->printFrame(PMTK_SET_NMEA_UPDATERATE, to_string(interval));
}
























