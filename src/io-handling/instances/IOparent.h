/*
 * IOParent.h
 *
 *  Created on: 19.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IOPARENT_H_
#define IOPARENT_H_

#include <mutex>
#include <sstream>
#include <string>

#include <cstdint>

typedef enum {
    IO_OK,
    IO_ERR_UNKNOWN,
    IO_ERR_OPEN,
    IO_ERR_SETUP,
    IO_ERR_WRITE,
    IO_ERR_READ,
    IO_ERR_RESET,
    IO_ERR_CLOSE
}ioStatus;

typedef enum {
    OPEN_R,
    OPEN_A,
    OPEN_W
}openType;

using namespace std;

class IOparent {
public:

    IOparent();
    virtual ~IOparent();

    // Setter/getter for 'active' member
    void setActive(bool active);
    bool isActive();

    // Pre-usage setup.
    virtual uint8_t open(uint8_t type, bool force=false)=0;
    virtual bool isOpen()=0;
    virtual uint8_t setup()=0;

    // Read functions.
    uint8_t readSafe(string &input, uint8_t delim);
    uint8_t readOpen(string &input, uint8_t delim);
    uint8_t read(string &input, uint8_t delim);

    // Write functions.
    uint8_t writeSafe(string output);
    uint8_t write(string output);

    // Append functions.
    uint8_t appendSafe(string output);
    uint8_t appendOpen(string output);
    uint8_t append(string output);

    // Post-usage setup.
    virtual uint8_t reset()=0;
    virtual uint8_t close()=0;

protected:

    virtual uint8_t getChar()=0;
    virtual uint8_t putString(string &output)=0;

private:

    // Activity indicator.
    bool active;

    // Read/write lock.
    mutex rw_mutex;

};

#endif /* IOPARENT_H_ */
