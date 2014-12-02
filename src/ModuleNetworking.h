/*
 * ModuleNetworking.h
 *
 *  Created on: 19.11.2014
 *      Author: administrator
 */

#ifndef MODULENETWORKING_H_
#define MODULENETWORKING_H_

#include "message-handling/msg/Msg.h"
#include "message-handling/MsgHub.h"
#include "network-handling/NetworkCommunicator.h"
#include "network-handling/NW_SocketInterface.h"
#include "network-handling/ProcPayload.h"
#include "network-handling/ProcDataFrame.h"
#include "Module.h"

#include <vector>

class ModuleNetworking : public Module {
public:
    ModuleNetworking();
    virtual ~ModuleNetworking();

protected:
    vector<shared_ptr<NetworkCommunicator>> communicators;

    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual shared_ptr<Msg> processMsg(shared_ptr<Msg>);
};

#endif /* MODULENETWORKING_H_ */
