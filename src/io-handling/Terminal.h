/*
 * Terminal.h
 *
 *  Created on: 24.03.2014
 *      Author: administrator
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "IOHandler.h"
#include "StrProcessor.h"
#include "../Child.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <ctime>

using namespace std;

class Terminal : public Child {
public:
	Terminal(IOHandler* handler);
	virtual ~Terminal();
	int run();
	int print();
	int scan();
    void addOutput(string str);
    int countInput();
    string getInput();
private:
	queue<string> input, output;
	IOHandler* hndl;
	mutex waitMutex, iMutex, oMutex;
	condition_variable condition;

    void addInput(string str);
};

#endif /* TERMINAL_H_ */
