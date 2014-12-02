/*
 * Message.cpp
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

#include "Msg.h"

Msg::Msg(int type) {
    this->mType = type;
}

Msg::~Msg() { }

int Msg::getType() {
    return this->mType;
}

void Msg::setType(int mType) {
    this->mType = mType;
}

MsgDataAcquired::MsgDataAcquired() : Msg(TYPE_DATA_ACQUIRED) { }

MsgDataAcquired::~MsgDataAcquired() { }

MsgDataAcquired* MsgDataAcquired::clone() {
    return new MsgDataAcquired(*this);
}

MsgDataSet::MsgDataSet() : Msg(TYPE_DATA_INCOMPLETE){

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_N, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_X, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Y, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Z, shared_ptr<ValVectorUChar>(new ValVectorUChar));

}

MsgDataSet::~MsgDataSet() { }

MsgDataSet* MsgDataSet::clone() {
    return new MsgDataSet(*this);
}
