#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#include <string>

using namespace std;

typedef enum ioStat{

	OP_OK,
	OP_UNKNOWN,
	OP_INACTIVE,
	OP_STREAM_EOF,
	OP_STREAM_NONFATAL,
	OP_STREAM_FATAL

}ioStat;

class IOHandler{
public:
	virtual ~IOHandler()=0;

	/* Read string from input stream. */
	virtual ioStat receive_s(string* string, int bytes=1)=0;

	/* Write string to output stream. */
	virtual ioStat send_s(string message)=0;

	/* Whether this handlers interface is active. */
	virtual bool isActive()=0;

	/* Set 'active' member */
	virtual void setActive(bool active=false)=0;

	/* Close in/out interface. */
	virtual void closeIO()=0;

	/* Copy method pointer */
	virtual IOHandler* clone()=0;
};

#endif /* IOHANDLER_H_ */
