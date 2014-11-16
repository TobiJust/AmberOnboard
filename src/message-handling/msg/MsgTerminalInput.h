/*
 * MsgTerminalInput.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef MSGTERMINALINPUT_H_
#define MSGTERMINALINPUT_H_

#include "Msg.h"

#include <string>

using namespace std;

class MsgTerminalInput : public Msg {
public:
    MsgTerminalInput(string str);
    virtual ~MsgTerminalInput();
    MsgTerminalInput* clone();
    string getInput();
private:
    string input;
};

#endif /* MSGTERMINALINPUT_H_ */
