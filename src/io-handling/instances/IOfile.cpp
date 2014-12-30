/*
 * IOfile.cpp
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "IOfile.h"

#include <iostream>
#include <iomanip>

IOfile::IOfile(string path) {
    this->path=path;
    this->file=NULL;
}

IOfile::~IOfile() {
    // TODO Auto-generated destructor stub
}

uint8_t IOfile::open(uint8_t type, bool force) {

    // File already open.
    if (this->isOpen()) {
        if (force)
            this->close();
        else
            return IO_OK;
    }

    // Determine kind of opening.
    switch (type) {
    case OPEN_W:
        this->file=fopen(this->path.c_str(),"w+");
        break;
    default:
    case OPEN_R:
    case OPEN_A:
        this->file=fopen(this->path.c_str(),"a+");
        break;
    }

    // An error occurred.
    if (!this->isOpen()) {
        this->file=NULL;
        return IO_ERR_OPEN;
    }

    this->setActive(true);

    return IO_OK;
}

bool IOfile::isOpen() {

    // File structure does not point to NULL and has a valid file descriptor.
    return ( this->file != NULL && fileno(this->file) > 0);
}

uint8_t IOfile::setup() {

    // No special setup needed.
    return IO_OK;
}

uint8_t IOfile::reset() {

    // No special setup needed.
    return IO_OK;
}

uint8_t IOfile::close() {

    // File already closed.
    if (!this->isOpen()) {
        this->file=NULL;
        return IO_OK;
    }

    // Close file and set file structure pointer to NULL.
    uint8_t result = fclose(this->file);
    file=NULL;

    // An error occurred.
    if ( result != 0)
        return IO_ERR_CLOSE;

    this->setActive(false);

    return IO_OK;
}

uint8_t IOfile::getChar() {

    // Return next character.
    return getc(this->file);

}

uint8_t IOfile::putString(string &output) {

    if (fputs(output.c_str(), this->file)==EOF)
        return IO_ERR_WRITE;

    return IO_OK;
}
