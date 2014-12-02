/*
 * ModuleNetworking.cpp
 *
 *  Created on: 19.11.2014
 *      Author: administrator
 */

#include "ModuleNetworking.h"

#include <iostream>

ModuleNetworking::ModuleNetworking() {

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
    addChildThread(realtime);
    realtime->attachObserver(this);

    MsgHub::getInstance()->attachObserverToMsg(this, TYPE_DATA_COMPLETE);

}

ModuleNetworking::~ModuleNetworking() {
    // TODO Auto-generated destructor stub
}

int ModuleNetworking::countMsgFromChildren() {

    // Return value.
    int result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Count pending input fields of standard input.
    while (commIt != this->communicators.end())
        result += (*commIt++)->countInput();

    return result;
}

int ModuleNetworking::pollMsgFromChildren() {

    // Return value.
    int result = 0;

    // Iterator for communicators.
    auto commIt = this->communicators.begin();

    // Poll pending input fields from each communicator.
    while (commIt != this->communicators.end()) {

        while ((*commIt)->countInput()) {

            shared_ptr<DataField> next = (*commIt)->getInput();

            switch (next->getType()) {
            case DATA_ACQUIRED:
            {
                this->attachChildToMsg(*commIt, TYPE_DATA_COMPLETE);
                shared_ptr<MsgDataAcquired> acquire(new MsgDataAcquired());
                MsgHub::getInstance()->appendMsg(acquire);
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

shared_ptr<Msg> ModuleNetworking::processMsg(shared_ptr<Msg> msg) {

    shared_ptr<Msg> result=NULL;

    switch (msg->getType()) {
    case TYPE_DATA_COMPLETE:
    {

        shared_ptr<Value> image, posE, posN, accX, accY, accZ;
        msg->getValue(ARG_IMG, image);
        msg->getValue(ARG_POS_E, posE);
        msg->getValue(ARG_POS_N, posN);
        msg->getValue(ARG_ACC_X, accX);
        msg->getValue(ARG_ACC_Y, accY);
        msg->getValue(ARG_ACC_Z, accZ);

        shared_ptr<AcquiredData_Out> outData(new AcquiredData_Out);
        outData->setValue(ARG_IMG, image);
        outData->setValue(ARG_POS_E, posE);
        outData->setValue(ARG_POS_N, posN);
        outData->setValue(ARG_ACC_X, accX);
        outData->setValue(ARG_ACC_Y, accY);
        outData->setValue(ARG_ACC_Z, accZ);

        auto childIt = this->getChildrenBegin(TYPE_DATA_COMPLETE);
        while (childIt != this->getChildrenEnd(TYPE_DATA_COMPLETE)) {

            shared_ptr<NetworkCommunicator> comm = dynamic_pointer_cast<NetworkCommunicator>(*childIt);
            comm->addOutput(outData);
            this->detachChildFromMsg(comm, TYPE_DATA_COMPLETE);
            childIt = this->getChildrenBegin(TYPE_DATA_COMPLETE);
        }
        break;
    }
    default:
        break;
    }

    return result;
}
