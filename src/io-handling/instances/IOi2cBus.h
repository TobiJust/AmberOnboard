/*
 * I2cBus.h
 *
 *  Created on: 22.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef I2CBUS_H_
#define I2CBUS_H_

#include "IOparent.h"

#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

using namespace std;

class IOi2cBus : public IOparent {
public:

    static shared_ptr<IOi2cBus> getInstance(string path);

    // Pre-usage setup.
    virtual uint8_t open(uint8_t type, bool force=false);
    virtual bool isOpen();
    virtual uint8_t setup();

    // Post-usage setup.
    virtual uint8_t reset();
    virtual uint8_t close();

    uint8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
    uint8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
    uint8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
    uint8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

    uint8_t writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    uint8_t writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
    uint8_t writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    uint8_t writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
    uint8_t writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    uint8_t writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

protected:

    virtual uint8_t getChar();
    virtual uint8_t putString(string &output);

private:

    IOi2cBus(string path);
    virtual ~IOi2cBus();

    // Custom deleter for shared pointer.
    friend class I2cBusDeleter;
    class I2cBusDeleter {
    public:
        void operator()(IOi2cBus* instance) { delete instance; }
    };

    uint8_t request(bool force=false, bool lock=false);
    uint8_t release(bool lock=false);

    string path;
    int32_t fileDesc;
    mutex busMutex;

    static unordered_map<string, shared_ptr<IOi2cBus>> i2cDevices;

};

#endif /* I2CBUS_H_ */
