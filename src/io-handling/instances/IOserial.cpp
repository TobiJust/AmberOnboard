/*
 * IOTerminal.cpp
 *
 *  Created on: 19.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "IOserial.h"

uint8_t IOserial::cmpBlock[] = {0};

IOserial::IOserial(string path, uint32_t baud) {

    this->file=NULL;
    this->path=path;
    this->setBaud(baud);
    memset(&this->backupConfig,0,sizeof(this->backupConfig));
    memset(&this->usedConfig,0,sizeof(this->usedConfig));
}

IOserial::~IOserial() {

    this->reset();
    this->close();
}

uint8_t IOserial::open(uint8_t type, bool force) {

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

    // Configure port.
    uint8_t status = this->setup();

    if (!status)
        this->setActive(true);

    // TODO: usleep!
    usleep(100);

    return status;
}

bool IOserial::isOpen() {

    // File structure does not point to NULL and has a valid file descriptor.
    return ( this->file != NULL && fileno(this->file) > 0);
}

uint8_t IOserial::setup() {


    // Check if backup configuration is initialized.
    if (!memcmp(&this->backupConfig, IOserial::cmpBlock, sizeof (termios)))
        if (tcgetattr(fileno(this->file), &this->backupConfig) == -1) {
            memset(&this->backupConfig, 0, sizeof (termios));
            return IO_ERR_SETUP;
        }

    // Create termios structure to work with, if necessary
    if (!memcmp(&this->usedConfig, IOserial::cmpBlock, sizeof (termios)))
        this->usedConfig=this->backupConfig;

    // Set up terminal.
    this->usedConfig.c_lflag &= ~( ICANON | ECHO ); // Disable canonical mode and local echo.
    this->usedConfig.c_cc[VMIN]=0;                  // Set minimum character count for reading.
    this->usedConfig.c_cc[VTIME]=0;                 // Set maximum time to wait for input.
    cfsetospeed(&this->usedConfig,this->baud);      // Set output baud rate.
    cfsetispeed(&this->usedConfig,this->baud);      // Set input baud rate.

    // Apply new settings.
    if (tcsetattr(fileno(this->file), TCSANOW, &this->usedConfig))
        return IO_ERR_SETUP;

    return IO_OK;
}

uint8_t IOserial::setBaud(uint32_t baud) {

    switch (baud) {
    case 50:        this->baud=B50;     break;
    case 75:        this->baud=B75;     break;
    case 110:       this->baud=B110;    break;
    case 134:       this->baud=B134;    break;
    case 150:       this->baud=B150;    break;
    case 200:       this->baud=B200;    break;
    case 300:       this->baud=B300;    break;
    case 600:       this->baud=B600;    break;
    case 1200:      this->baud=B1200;   break;
    case 1800:      this->baud=B1800;   break;
    case 2400:      this->baud=B2400;   break;
    case 4800:      this->baud=B4800;   break;
    case 9600:      this->baud=B9600;   break;
    case 19200:     this->baud=B19200;  break;
    case 38400:     this->baud=B38400;  break;
    case 57600:     this->baud=B57600;  break;
    case 115200:    this->baud=B115200; break;
    case 230400:    this->baud=B230400; break;
    case 460800:    this->baud=B460800; break;
    default:        this->baud=B38400;
    return IO_ERR_SETUP;
    }

    uint8_t status = IO_OK;

    // Set up port.
    if (this->isOpen())
        status = this->setup();

    return status;
}

uint32_t IOserial::getBaud() {

    switch (this->baud) {
    case B50:       return 50;
    case B75:       return 75;
    case B110:      return 110;
    case B134:      return 134;
    case B150:      return 150;
    case B200:      return 200;
    case B300:      return 300;
    case B600:      return 600;
    case B1200:     return 1200;
    case B1800:     return 1800;
    case B2400:     return 2400;
    case B4800:     return 4800;
    case B9600:     return 9600;
    case B19200:    return 19200;
    case B38400:    return 38400;
    case B57600:    return 57600;
    case B115200:   return 115200;
    case B230400:   return 230400;
    case B460800:   return 460800;
    default:        break;
    }

    return 0;
}

uint8_t IOserial::reset() {

    // check if backup configuration is initialized.
    if (memcmp(&this->backupConfig, IOserial::cmpBlock, sizeof (termios))) {

        // Apply old settings.
        if (tcsetattr(fileno(this->file), TCSANOW, &this->backupConfig))
            return IO_ERR_RESET;
    }else
        return IO_ERR_RESET;

    return IO_OK;
}

uint8_t IOserial::close() {

    // File already closed.
    if (!this->isOpen()) {
        this->file=NULL;
        return IO_OK;
    }

    // Restore old port settings.
    uint8_t status = this->reset();

    // An error occurred.
    if(status)
        return status;

    // Close file and set file structure pointer to NULL.
    uint8_t result = fclose(this->file);
    file=NULL;

    // An error occurred.
    if ( result != 0)
        return IO_ERR_CLOSE;

    this->setActive(false);

    return IO_OK;
}

uint8_t IOserial::getChar() {

    uint8_t result=EOF;

    while((result = getc(this->file)) == (uint8_t)EOF && this->isActive())

        // Pause for half baud cycle.
        usleep(1000000 / (2*this->getBaud()));

    // Return next character.
    return result;

}

uint8_t IOserial::putString(string &output) {

    if (fputs(output.c_str(), this->file)==EOF)
        return IO_ERR_WRITE;

    // Wait until data is sent.
    if (tcdrain(fileno(this->file)))
        return IO_ERR_WRITE;

    return IO_OK;
}






















