/*
 * IOfile.h
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IOFILE_H_
#define IOFILE_H_

#include "IOparent.h"

#include <fstream>
#include <string>

#include <cstdio>

#include <sys/stat.h>

using namespace std;

class IOfile : public IOparent {
public:

    IOfile(string path);
    virtual ~IOfile();

    bool exists();
    string getPath();

    // Pre-usage setup.
    virtual uint8_t open(uint8_t type, bool force=false);
    virtual bool isOpen();
    virtual uint8_t setup();

    // Post-usage setup.
    virtual uint8_t reset();
    virtual uint8_t close();

protected:

    virtual uint8_t getChar();
    virtual uint8_t putString(string &output);

private:

    // Relativ or absolute target path in file system.
    string path;

    // File representation.
    FILE *file;

};

#endif /* IOFILE_H_ */
