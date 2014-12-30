/*
 * Terminal.h
 *
 *  Created on: 29.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "instances/IOserial.h"
#include "../Child.h"

#include <thread>

typedef enum {
    TERM_OK,
    TERM_ERR_OPEN,
}termState;

class Terminal : public Child {
public:

    Terminal(string path, uint32_t baud);
    virtual ~Terminal();
    uint8_t initialize();

    virtual int run();
    int print();
    int scan();

private:

    IOserial port;

    string getTime();

};

#endif /* TERMINAL_H_ */
