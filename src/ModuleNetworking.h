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

typedef enum {
    NW_MOD_OK,
    NW_MOD_ERR_UNKNOWN,
    NW_MOD_ERR_INVALID_REFERENCE
} nwProcStatus;

class ModuleNetworking : public Module {
public:

    ModuleNetworking();
    virtual ~ModuleNetworking();

    uint8_t com_append(shared_ptr<NetworkCommunicator> executor);
    void com_delete(shared_ptr<NetworkCommunicator> com);
    void com_clear();

protected:
    vector<shared_ptr<NetworkCommunicator>> communicators;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULENETWORKING_H_ */
