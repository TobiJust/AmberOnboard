/*
 * FileIO.h
 *
 *  Created on: 04.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef FILEIO_H_
#define FILEIO_H_

#include <mutex>
#include <string>
#include <stdint.h>
#include <termios.h>

using namespace std;

class FileIO {
public:
    FileIO(string path);
    virtual ~FileIO();

    /* Read string from input stream. */
    virtual uint8_t read(string &input);

    /* append string to output stream. */
    virtual uint8_t append(string &output);

    /* Write string to output stream. */
    virtual uint8_t write(string &output);

    /* Whether this handlers interface is active. */
    virtual bool isActive();

    /* Set 'active' member */
    virtual bool setActive(bool active=true);

private:

    struct termios t_Standard, t_Nonblock;

    // Read/write mutex.
    mutex rw_Mutex;

    // Input/Output descriptors.
    FILE *out_Desc, *in_Desc;

    // Relativ or absolute target path in file system.
    string path;

    // Close read/write access to file.
    virtual void closeFile();
};

#endif /* FILEIO_H_ */
