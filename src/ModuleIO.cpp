/*
 * ModuleIO.cpp
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#include "ModuleIO.h"

ModuleIO::ModuleIO() {

    // Register for message types.
    MsgHub::getInstance()->attachObserverToMsg(this, TYPE_TERM_INPUT);

    createStreamTerminal(cin, cout);
}

ModuleIO::~ModuleIO() {
    // TODO Auto-generated destructor stub
}

void ModuleIO::createStreamTerminal(istream& input, ostream& output) {

    // Create StreamIO instance for handling stream input/output.
    StreamIO s_io(input, output);

    // Polymorph (IOHandler) interface pointer to StreamIO.
    IOHandler *s_ioH = &s_io;

    // Create and set up Terminal instance.
    createTerminal(s_ioH);

}

void ModuleIO::createTerminal(IOHandler* hndl) {

    // Create new Terminal instance, using IOHandler reference.
    term.reset(new Terminal(hndl));
    term->attachObserver(this);

    // Add child to list of joinable threads.
    addChildThread(term);

}

int ModuleIO::countMsgFromChildren() {

    // Return value.
    int result = 0;

    // Count pending strings of standard input.
    result += term->countInput();

    return result;
}

int ModuleIO::pollMsgFromChildren() {

    // Return value.
    int result = 0;

    while(term->countInput()){

        string next = term->getInput();

        shared_ptr<MsgTerminalInput> input(new MsgTerminalInput(next));
        MsgHub::getInstance()->appendMsg(input);

        result++;

    }

    return result;
}

shared_ptr<Msg> ModuleIO::processMsg(shared_ptr<Msg> msg) {

    shared_ptr<Msg> result=NULL;

    switch (msg->getType()) {
    case TYPE_TERM_INPUT:
    {
        shared_ptr<MsgTerminalInput> instance = dynamic_pointer_cast<MsgTerminalInput>(msg);
        term->addOutput(instance->getInput());
        break;
    }
    default:
        break;
    }

    return result;
}
