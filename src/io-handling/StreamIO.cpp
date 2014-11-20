/*
 * StreamIO.cpp
 *
 *  Created on: 11.03.2014
 *      Author: administrator
 */

#include "StreamIO.h"

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

    // Read from member stream.
    inStream->read(buffer, bytes);

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

    this->active = active;
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
