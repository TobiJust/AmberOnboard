/*
 * Module.h
 *
 *  Created on: 31.10.2014
 *      Author: administrator
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "Child.h"
#include "message-handling/MsgHub.h"
#include "message-handling/Observer.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>

using namespace std;

class Module : public Observer {
public:

    // Instantiation.
    Module();
    virtual ~Module();
    thread* createThread();
    int run();

    // Module properties.
    string getName();

    // Message processing.
    void update();

protected:

    // Data members.
    condition_variable condition;
    mutex waitMutex;
    queue<Msg*> sendBuf;
    unordered_set<thread*> childThreads;

    // Message processing.
    bool msgAvailable();
    virtual int countMsgFromChildren()=0;
    virtual int pollMsgFromChildren()=0;
    int pollMsgFromHub();
    virtual Msg* processMsg(Msg*)=0;

    void addChildThread(Child* child);

};

#endif /* MODULE_H_ */
