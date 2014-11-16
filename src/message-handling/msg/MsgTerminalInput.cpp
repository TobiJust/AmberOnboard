/*
 * MsgTerminalInput.cpp
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#include "MsgTerminalInput.h"

MsgTerminalInput::MsgTerminalInput(string str) : Msg(TYPE_TERM_INPUT) {
    this->input=str;
}

MsgTerminalInput::~MsgTerminalInput() { }

string MsgTerminalInput::getInput() {
    return input;
}

MsgTerminalInput* MsgTerminalInput::clone() {
    return new MsgTerminalInput(*this);
}
