/*
 * FileIO.cpp
 *
 *  Created on: 04.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "FileIO.h"

FileIO::FileIO(string path) {

    this->path=path;
    this->in_Desc=NULL;
    this->out_Desc=NULL;
}

FileIO::~FileIO() {

    closeFile();
}

uint8_t FileIO::read(string &input) {

    this->rw_Mutex.lock();

    // Input descriptor NULL.
    if (!this->in_Desc) {

        // Try opening the file path in read mode.
        this->in_Desc=fopen(this->path.c_str(),"r");

        // Get standard terminal attributes for later restoration.
        tcgetattr(fileno(this->in_Desc), &(this->t_Standard));

        // Create termios in nonblocking mode.
        this->t_Nonblock=this->t_Standard;
        this->t_Nonblock.c_lflag &= ~(ICANON);

    }

    // Input descriptor not NULL and position indicator valid.
    if (this->in_Desc && ftell(this->in_Desc) != -1L) {

        tcsetattr(fileno(this->in_Desc), TCSANOW, &(this->t_Nonblock));  // apply new settings

        int c = getchar();  // read character (non-blocking)

        tcsetattr(fileno(this->in_Desc), TCSANOW, &(this->t_Standard));  // apply new settings
    }

    this->rw_Mutex.unlock();

    return 0;
}

uint8_t FileIO::append(string &output) {
    return 0;
}

uint8_t FileIO::write(string &output) {

    this->rw_Mutex.lock();

    // Output descriptor NULL.
    if (!this->out_Desc) {

        // Try opening the file path in read mode.
        this->out_Desc=fopen(this->path.c_str(),"w");

        // Since a new write session has startet,
        // the input descriptor has to be reopened, too.
        fclose(this->in_Desc);
        this->in_Desc=fopen(this->path.c_str(),"r");

    }

    // Output descriptor not NULL and position indicator valid.
    if (this->out_Desc && ftell(this->out_Desc) != -1L) {

    }



    this->in_Desc=fopen("/dev/tty","w");

    this->rw_Mutex.unlock();
    return 0;
}

bool FileIO::isActive() {
    return true;
}

bool FileIO::setActive(bool active) {

    if (active) {

    } else
        closeFile();

    return false;

}

void FileIO::closeFile() {

    // If file is opened for input at all.
    if (this->in_Desc != NULL) {

        // Close input descriptor and set it to NULL.
        fclose(this->in_Desc);
        this->in_Desc=NULL;
    }

    // If file is opened for output at all.
    if (this->out_Desc != NULL) {

        // Close output descriptor and set it to NULL.
        fclose(this->out_Desc);
        this->out_Desc=NULL;
    }
}
