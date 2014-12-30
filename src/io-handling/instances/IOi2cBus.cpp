/*
 * I2cBus.cpp
 *
 *  Created on: 22.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "IOi2cBus.h"

unordered_map<string, shared_ptr<IOi2cBus>> IOi2cBus::i2cDevices;

IOi2cBus::IOi2cBus(string path) {

    this->busMutex.lock();

    this->path = path;
    this->fileDesc = -1;

    this->busMutex.unlock();
}

IOi2cBus::~IOi2cBus() { }

shared_ptr<IOi2cBus> IOi2cBus::getInstance(string path) {

    // Bus instance.
    shared_ptr<IOi2cBus> result;

    // Find path in already managed i2c interfaces.
    auto busIt = i2cDevices.find(path);

    // Bus already managed.
    if ( busIt != i2cDevices.end() )
        result = busIt->second;

    // Bus not managed yet.
    else {

        result=shared_ptr<IOi2cBus>(new IOi2cBus(path), IOi2cBus::I2cBusDeleter());
        i2cDevices.insert(make_pair(path, result));
    }

    return result;
}

uint8_t IOi2cBus::open(uint8_t type, bool force) {
    return request(force);
}

uint8_t IOi2cBus::request(bool force, bool lock) {

    if ( lock )
        this->busMutex.lock();

    // Bus already open.
    if (this->isOpen()) {
        if (force)
            this->release(false);
        else
            return IO_OK;
    }

    // Now try opening bus.
    this->fileDesc = ::open(this->path.c_str(), O_RDWR);

    // An error occurred while opening.
    if ( !this->isOpen() ){

        this->fileDesc = -1;

        if ( lock )
            this->busMutex.unlock();

        return IO_ERR_OPEN;
    }

    if ( lock )
        this->busMutex.unlock();

    return IO_OK;
}

bool IOi2cBus::isOpen() {
    return this->fileDesc != -1 && fcntl(this->fileDesc, F_GETFL) >= 0 && errno != EBADF;
}

uint8_t IOi2cBus::setup() { return IO_OK; }
uint8_t IOi2cBus::reset() { return IO_OK; }

uint8_t IOi2cBus::close() {
    return release();
}

uint8_t IOi2cBus::release(bool lock) {

    if ( lock )
        this->busMutex.lock();

    // Bus already open.
    if( this->isOpen() ) {

        uint8_t status = ::close(this->fileDesc);

        // An error occurred.
        if ( status != 0) {

            if ( lock )
                this->busMutex.unlock();

            return IO_ERR_CLOSE;
        }
    }

    if ( lock )
        this->busMutex.unlock();

    return IO_OK;
}

uint8_t IOi2cBus::getChar() {

    uint8_t result=EOF;

    // Read character from i2c bus.
    ::read(this->fileDesc, &result, 1);

    return result;

}

uint8_t IOi2cBus::putString(string &output) {

    // Write output to i2c bus.
    if ( ::write(this->fileDesc, output.c_str(), output.length()) != 1 )
        return IO_ERR_WRITE;

    return IO_OK;
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status indicator of read operation.
 */
uint8_t IOi2cBus::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t status = readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return status;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status indicator of read operation.
 */
uint8_t IOi2cBus::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t status, b;
    if ( !(status = readByte(devAddr, regAddr, &b)) ) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return status;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status indicator of read operation.
 */
uint8_t IOi2cBus::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    return readBytes(devAddr, regAddr, 1, data);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @return Status indicator of read operation.
 */
uint8_t IOi2cBus::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {

    this->busMutex.lock();

    uint8_t status=IO_OK;

    // Open bus if necessary.
    if ( !this->isOpen() )
        status = this->request();

    // An error occurred.
    if ( status ) {
        this->busMutex.unlock();
        return status;
    }

    // Select i2c slave address.
    if ( ioctl(this->fileDesc, I2C_SLAVE, devAddr) < 0 ) {
        this->busMutex.unlock();
        return IO_ERR_SETUP;
    }

    // Write requested register to i2c bus.
    if ( ::write(this->fileDesc, &regAddr, 1) != 1 ) {
        this->busMutex.unlock();
        return IO_ERR_WRITE;
    }

    // Read data from i2c bus.
    if ( ::read(this->fileDesc, data, length) != length )
        status=IO_ERR_READ;

    this->busMutex.unlock();

    return status;
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if ( !readByte(devAddr, regAddr, &b) ) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(devAddr, regAddr, b);
    } else {
        return IO_ERR_READ;
    }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return writeBytes(devAddr, regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
    return writeWords(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {

    uint8_t buf[128];

    // Should do potential byteswap and call writeBytes() really, but that
    // messes with the callers buffer

    if (length > 127)
        return IO_ERR_UNKNOWN;

    this->busMutex.lock();

    uint8_t status=IO_OK;

    // Open bus if necessary.
    if ( !this->isOpen() )
        status = this->request();

    // An error occurred.
    if ( status ) {
        this->busMutex.unlock();
        return status;
    }

    // Select i2c slave address.
    if ( ioctl(this->fileDesc, I2C_SLAVE, devAddr) < 0 ) {
        this->busMutex.unlock();
        return IO_ERR_SETUP;
    }

    // Prepare written data.
    buf[0] = regAddr;
    memcpy(buf+1,data,length);

    // Write bytes to i2c bus.
    if (::write(this->fileDesc, buf, length+1) != length+1)
        status=IO_ERR_WRITE;

    this->busMutex.unlock();

    return status;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
uint8_t IOi2cBus::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {

    uint8_t buf[128];

    // Should do potential byteswap and call writeBytes() really, but that
    // messes with the callers buffer

    if (length > 63)
        return IO_ERR_UNKNOWN;

    this->busMutex.lock();

    uint8_t status=IO_OK;

    // Open bus if necessary.
    if ( !this->isOpen() )
        status = this->request();

    // An error occurred.
    if ( status ) {
        this->busMutex.unlock();
        return status;
    }

    // Select i2c slave address.
    if ( ioctl(this->fileDesc, I2C_SLAVE, devAddr) < 0 ) {
        this->busMutex.unlock();
        return IO_ERR_SETUP;
    }

    // Convert 2-byte words to 1-byte ones.
    buf[0] = regAddr;
    for (uint8_t i = 0; i < length; i++) {
        buf[i*2+1] = data[i] >> 8;
        buf[i*2+2] = data[i];
    }

    // Write bytes to i2c bus.
    if (::write(this->fileDesc, buf, length*2+1) != length*2+1)
        status=IO_ERR_WRITE;

    this->busMutex.unlock();

    return status;
}

