/*
 * StreamIO.cpp
 *
 *  Created on: 11.03.2014
 *      Author: Daniel Wagenknecht
 */

#include "StreamIO.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>

using namespace std;

StreamIO::StreamIO(istream& input, ostream& output) {

    this->inStream = &input;
    this->outStream = &output;
    this->active = true;

}

StreamIO::~StreamIO() { }

ioStat StreamIO::receive_s(string* target, int bytes) {

    if (!active)
        return OP_INACTIVE;

    // Clear all stream flags.
    inStream->clear();

    // Target character buffer.
    char* buffer = new char[bytes + 1];



    static struct termios oldt, newt;
      tcgetattr( STDIN_FILENO, &oldt);           // save old settings
      newt = oldt;
      newt.c_lflag &= ~(ICANON);                 // disable buffering
      tcsetattr( STDIN_FILENO, TCSANOW, &newt);  // apply new settings

      int c = getchar();  // read character (non-blocking)

      tcsetattr( STDIN_FILENO, TCSANOW, &oldt);  // restore old settings






    cerr << "StreamIO: reading some bytes" << endl;;

    // Read from member stream.
    inStream->read(buffer, bytes);

    cerr << "StreamIO: read some bytes" << endl;;

    // None of the error/failure flags was set.
    if (inStream->good()) {

        // Append freshly read characters and return status indicator.
        target->append(buffer);
        return OP_OK;

    } else {  // A problem occurred.
        this->active = false;

        // Base class pointer.
        basic_ios<char> *basicIn = inStream;
        return StreamIO::getStat(basicIn);
    }

}

ioStat StreamIO::send_s(string message) {

    if (!active)
        return OP_INACTIVE;

    *(this->outStream) << message << endl;

    // None of the error/failure flags was set.
    if (outStream->good())
        return OP_OK;

    else {  // A problem occurred.
        this->active = false;

        // Base class pointer.
        basic_ios<char> *basicOut = outStream;
        return StreamIO::getStat(basicOut);
    }
}

bool StreamIO::isActive() {

    return active;
}

void StreamIO::setActive(bool active) {

    cerr << "StreamIO: setting to " << (active?"active":"inactive") << endl;

    this->active = active;
    if (this->active)
        this->inStream->clear(ios::goodbit);
    else {
        this->inStream->setstate(ios::eofbit);
        this->inStream->putback('\n');
    }
}

ioStat StreamIO::getStat(basic_ios<char>* stream) {

    // End of stream reached.
    if (stream->eof())
        return OP_STREAM_EOF;

    // Non-fatal issue occurred.
    else if (stream->fail())
        return OP_STREAM_NONFATAL;

    // Fatal issue occurred.
    else if (stream->bad())
        return OP_STREAM_FATAL;

    // Unknown stream state.
    return OP_UNKNOWN;
}

StreamIO* StreamIO::clone(){
    return new StreamIO(*this);
}

void StreamIO::closeIO() { }
