/*
 * IOParent.cpp
 *
 *  Created on: 19.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "IOparent.h"

#include <iostream>
#include <iomanip>

IOparent::IOparent() {
    this->active=false;
}

IOparent::~IOparent() { }

void IOparent::setActive(bool active) {
    this->active=active;
}

bool IOparent::isActive() {
    return this->active;
}

uint8_t IOparent::readSafe(string &input, uint8_t delim) {

    // Read input.
    uint8_t status = this->readOpen(input, delim);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

uint8_t IOparent::readOpen(string &input, uint8_t delim) {

    uint8_t status = IO_OK;

    // Open file if not already open:
    if (!this->isOpen())
        status = this->open(OPEN_R);

    // An error occurred.
    if (status)
        return status;

    status = this->read(input, delim);

    return status;

}

uint8_t IOparent::read(string &input, uint8_t delim) {

    if (!this->isOpen())
        return IO_ERR_OPEN;

    // Resulting stringstream.
    stringstream stream("");

    // Last received byte.
    uint8_t current=0;

    this->rw_mutex.lock();

    // Read characters until delimiter is found.
    do {
        current = this->getChar();
        stream << current;
    } while (current != delim && this->active);

    this->rw_mutex.unlock();

    // Set resulting string.
    input=stream.str();

    return IO_OK;

}

uint8_t IOparent::writeSafe(string output) {

    uint8_t status = write(output);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

uint8_t IOparent::write(string output) {

    uint8_t status = IO_OK;

    // Open file anyway.
    // if (!this->isOpen())
        status = this->open(OPEN_W, true);

    // An error occurred.
    if (status)
        return status;

    status = this->append(output);

    return status;

}

uint8_t IOparent::appendSafe(string output) {

    uint8_t status = this->appendOpen(output);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

uint8_t IOparent::appendOpen(string output) {

    uint8_t status = IO_OK;

    // Open file if not already open:
    if (!this->isOpen())
        status = this->open(OPEN_A);

    // An error occurred.
    if (status)
        return status;

    status = this->append(output);

    return status;

}

uint8_t IOparent::append(string output) {

    this->rw_mutex.lock();

    uint8_t status = this->putString(output);

    this->rw_mutex.unlock();

    return status;

}
