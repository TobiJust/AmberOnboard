/*
 * ModuleIO.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef MODULEIO_H_
#define MODULEIO_H_

#include "Module.h"
#include "io-handling/IOHandler.h"
#include "io-handling/StreamIO.h"
#include "io-handling/Terminal.h"
#include "message-handling/msg/MsgTerminalInput.h"

#include <iostream>

using namespace std;

class ModuleIO : public Module {
public:
    ModuleIO();
    virtual ~ModuleIO();

    void createStreamTerminal(istream& input, ostream& output);
    void createTerminal(IOHandler* hndl);

protected:
    Terminal *term;

    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);
};

#endif /* MODULEIO_H_ */
