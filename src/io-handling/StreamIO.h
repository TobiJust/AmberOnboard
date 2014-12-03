/*
 * StreamIO.h
 *
 *  Created on: 11.03.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef STREAMIO_H_
#define STREAMIO_H_

#include "IOHandler.h"
#include <istream>
#include <ostream>

using namespace std;

class StreamIO: public IOHandler {
public:
	StreamIO(istream& input, ostream& output);
	~StreamIO();
	ioStat receive_s(string* string, int bytes=1);
	ioStat send_s(string message);
	bool isActive();
	void setActive(bool active = false);
	StreamIO* clone();
    void closeIO();
private:
	istream* inStream;
	ostream* outStream;
	bool active;
	static ioStat getStat(basic_ios<char>* stream);
};

#endif /* STREAMIO_H_ */
