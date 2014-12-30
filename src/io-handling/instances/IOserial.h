/*
 * IOTerminal.h
 *
 *  Created on: 19.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IOTERMINAL_H_
#define IOTERMINAL_H_

#include "IOparent.h"

#include <string>

#include <cstdio>
#include <cstring>

#include <termios.h>
#include <unistd.h>

using namespace std;

class IOserial : public IOparent {
public:

    IOserial(string path, uint32_t baud);
    virtual ~IOserial();

    // Pre-usage setup.
    virtual uint8_t open(uint8_t type, bool force=false);
    virtual bool isOpen();
    virtual uint8_t setup();

    // Set/get terminal baud rate.
    uint8_t setBaud(uint32_t baud);
    uint32_t getBaud();

    // Post-usage setup.
    virtual uint8_t reset();
    virtual uint8_t close();

protected:

    virtual uint8_t getChar();
    virtual uint8_t putString(string &output);

private:

    // Relativ or absolute target path in file system.
    string path;

    // Terminal baud rate.
    speed_t baud;

    // File representation.
    FILE *file;

    // Terminal structures.
    struct termios backupConfig, usedConfig;
    static uint8_t cmpBlock[sizeof (termios)];

};

#endif /* IOTERMINAL_H_ */
