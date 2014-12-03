/*
 * ModuleIO.cpp
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ModuleIO.h"

ModuleIO::ModuleIO() {

    // Register for message types.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_IN);

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
    addChild(term);

}

uint8_t ModuleIO::countMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Count pending strings of standard input.
    result += term->countInput();

    return result;
}

uint8_t ModuleIO::pollMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    while(term->countInput()){

        shared_ptr<ValString> inputString(new ValString(term->getInput()));
        shared_ptr<M2M_TerminalInput> input(new M2M_TerminalInput);
        input->setValue(ARG_TERM_IN, inputString);
        MsgHub::getInstance()->appendMsg(input);

        result++;

    }

    return result;
}

shared_ptr<Message_M2M> ModuleIO::processMsg(shared_ptr<Message_M2M> msg) {

    shared_ptr<Message_M2M> result=NULL;

    switch (msg->getType()) {
    case MSG_TERM_IN:
    {
        shared_ptr<M2M_TerminalInput> instance = dynamic_pointer_cast<M2M_TerminalInput>(msg);
        shared_ptr<Value> input;
        instance->getValue(ARG_TERM_IN, input);

        shared_ptr<M2C_TerminalOutput> outData(new M2C_TerminalOutput);
        outData->setValue(ARG_TERM_OUT, input);

        term->out_push(dynamic_pointer_cast<Message_M2C>(outData));
        break;
    }
    default:
        break;
    }

    return result;
}
