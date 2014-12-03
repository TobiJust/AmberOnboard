/*
 * ModuleNetworking.h
 *
 *  Created on: 19.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULENETWORKING_H_
#define MODULENETWORKING_H_

#include "msg-handling/Msg.h"
#include "msg-handling/MsgHub.h"
#include "nw-handling/NetworkCommunicator.h"
#include "nw-handling/NW_SocketInterface.h"
#include "nw-handling/ProcPayload.h"
#include "nw-handling/ProcDataFrame.h"
#include "Module.h"

#include <vector>

class ModuleNetworking : public Module {
public:
    ModuleNetworking();
    virtual ~ModuleNetworking();

protected:
    vector<shared_ptr<NetworkCommunicator>> communicators;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULENETWORKING_H_ */
