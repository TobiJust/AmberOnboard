/*
 * ModuleImgProcessing.cpp
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#include "ModuleImgProcessing.h"

ModuleImgProcessing::ModuleImgProcessing() {
    // TODO Auto-generated constructor stub

}

ModuleImgProcessing::~ModuleImgProcessing() {
    // TODO Auto-generated destructor stub
}

int ModuleImgProcessing::countMsgFromChildren() {

    // Return value.
    int result = 0;

    return result;
}

int ModuleImgProcessing::pollMsgFromChildren() {

    // Return value.
    int result = 0;

    return result;
}

Msg* ModuleImgProcessing::processMsg(Msg* msg) {

    Msg* result=NULL;

    switch (msg->getType()) {
    case TYPE_TERM_INPUT:
        break;
    default:
        break;
    }

    return result;
}
