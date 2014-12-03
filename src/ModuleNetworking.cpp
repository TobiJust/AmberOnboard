/*
 * ModuleNetworking.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ModuleNetworking.h"

#include <iostream>

ModuleNetworking::ModuleNetworking() {

    // Real time connection
    shared_ptr<NW_SocketInterface> interface(new NW_SocketInterface);
    interface->setValue(ARG_IP_FAMILY, shared_ptr<ValInt>(new ValInt(AF_UNSPEC)));
    interface->setValue(ARG_SOCK_TYPE, shared_ptr<ValInt>(new ValInt(SOCK_STREAM)));
    interface->setValue(ARG_TARGET_ADDR, shared_ptr<ValString>(new ValString("localhost")));
    interface->setValue(ARG_TARGET_PORT, shared_ptr<ValString>(new ValString("5555")));
    interface->initialize();

    shared_ptr<ProcDataFrame> frame(new ProcDataFrame);
    shared_ptr<ProcPayload> payload(new ProcPayload);
    payload->setValue(ARG_DEV_ID, shared_ptr<ValInt>(new ValInt(123)));

    shared_ptr<NetworkCommunicator> realtime(new NetworkCommunicator);
    realtime->appenProc(payload);
    realtime->appenProc(frame);
    realtime->appenProc(interface);
    this->communicators.push_back(realtime);

    // Add child to list of joinable threads.
    addChild(realtime);
    realtime->attachObserver(this);

    MsgHub::getInstance()->attachObserverToMsg(this, MSG_DATA_COMPLETE);

}

ModuleNetworking::~ModuleNetworking() {
    // TODO Auto-generated destructor stub
}

uint8_t ModuleNetworking::countMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Count pending input fields of standard input.
    while (commIt != this->communicators.end())
        result += (*commIt++)->in_count();

    return result;
}

uint8_t ModuleNetworking::pollMsgFromChildren() {

    // Return value.
    uint8_t result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Poll pending input fields from each communicator.
    while (commIt != this->communicators.end()) {

        while ((*commIt)->in_count()) {

            shared_ptr<Message_M2C> next = (*commIt)->in_pop();

            switch (next->getType()) {
            case MSG_DATA_ACQUIRED:
            {

                string next = "Acquired\n";
                shared_ptr<ValString> inputString(new ValString(next));
                shared_ptr<M2M_TerminalInput> input(new M2M_TerminalInput);
                input->setValue(ARG_TERM_IN, inputString);
                MsgHub::getInstance()->appendMsg(input);

                this->attachChildToMsg(*commIt, MSG_DATA_COMPLETE);
                shared_ptr<M2M_DataAcquired> acquire(new M2M_DataAcquired());
                MsgHub::getInstance()->appendMsg(acquire);
                /*
                 */

                break;
            }

            default:
                break;
            }
        }
        commIt++;
    }

    return result;
}

shared_ptr<Message_M2M> ModuleNetworking::processMsg(shared_ptr<Message_M2M> msg) {

    shared_ptr<Message_M2M> result=NULL;

    switch (msg->getType()) {
    case MSG_DATA_COMPLETE:
    {
        shared_ptr<Value> image, posE, posN, accX, accY, accZ;
        msg->getValue(ARG_IMG, image);
        msg->getValue(ARG_POS_E, posE);
        msg->getValue(ARG_POS_N, posN);
        msg->getValue(ARG_ACC_X, accX);
        msg->getValue(ARG_ACC_Y, accY);
        msg->getValue(ARG_ACC_Z, accZ);

        shared_ptr<M2C_DataSet> outData(new M2C_DataSet);
        outData->setValue(ARG_IMG, image);
        outData->setValue(ARG_POS_E, posE);
        outData->setValue(ARG_POS_N, posN);
        outData->setValue(ARG_ACC_X, accX);
        outData->setValue(ARG_ACC_Y, accY);
        outData->setValue(ARG_ACC_Z, accZ);
        outData->setType(MSG_DATA_COMPLETE);

        auto childIt = this->getChildrenBegin(MSG_DATA_COMPLETE);
        while (childIt != this->getChildrenEnd(MSG_DATA_COMPLETE)) {

            shared_ptr<NetworkCommunicator> comm = dynamic_pointer_cast<NetworkCommunicator>(*childIt);
            comm->out_push(dynamic_pointer_cast<Message_M2C>(outData));
            this->detachChildFromMsg(comm, MSG_DATA_COMPLETE);
            childIt = this->getChildrenBegin(MSG_DATA_COMPLETE);
        }
        break;
    }
    default:
        break;
    }

    return result;
}
