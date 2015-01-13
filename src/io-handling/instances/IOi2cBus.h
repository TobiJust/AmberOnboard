// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 8/24/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

/** \brief      Gneral class for i2c access.
 *
 * \details     This class represents all i2c busses and offers methods for access.
 *              Original by Jeff Rowberg, modified by Daniel Wagenknecht
 * \author      Daniel Wagenknecht
 * \version     2014-12-22
 * \class       IOi2cBus
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
