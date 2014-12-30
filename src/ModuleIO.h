/*
 * ModuleIO.h
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULEIO_H_
#define MODULEIO_H_

#include "Module.h"
#include "io-handling/Config.h"
// #include "io-handling/IOHandler.h"
// #include "io-handling/StreamIO.h"
// #include "io-handling/Terminal.h"

#include <iostream>

using namespace std;

class ModuleIO : public Module {
public:
    ModuleIO();
    virtual ~ModuleIO();

    virtual bool setup();

    void createStreamTerminal(istream& input, ostream& output);
    // void createTerminal(shared_ptr<IOHandler> hndl);

protected:

    // Config conf;
    // shared_ptr<Terminal> term;

    void createTerminal(string path, uint32_t baud);

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULEIO_H_ */
