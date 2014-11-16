/*
 * Message.cpp
 *
 *  Created on: 31.10.2014
 *      Author: administrator
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
