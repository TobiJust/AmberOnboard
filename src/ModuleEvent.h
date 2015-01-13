/*
 * ModuleEvent.h
 *
 *  Created on: 07.01.2015
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULEEVENT_H_
#define MODULEEVENT_H_

#include "msg-handling/Msg.h"
#include "msg-handling/MsgHub.h"
#include "Module.h"
#include "evt-handling/EvtProcessor.h"

typedef enum {
    EVT_MOD_OK,
    EVT_MOD_ERR_UNKNOWN,
    EVT_MOD_ERR_INVALID_REFERENCE
} evtProcStatus;

class ModuleEvent : public Module {
public:

    ModuleEvent();
    virtual ~ModuleEvent();

    uint8_t evt_append(shared_ptr<EvtProcessor> processor);
    void evt_clear();

protected:
    vector<shared_ptr<EvtProcessor>> processors;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULEEVENT_H_ */
