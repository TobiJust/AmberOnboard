/*
 * MsgSetConfig.cpp
 *
 *  Created on: 09.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "MsgConfig.h"

MsgConfig::MsgConfig(int msgType, int confGroup) : Msg(msgType) {
    this->confGroup=confGroup;
}

MsgConfig::~MsgConfig() { }

void MsgConfig::addOption(Opt* option) {
    options.push(option);
}

Opt* MsgConfig::getNextOption() {

    Opt* result = NULL;

    if(!options.empty()){
        result = options.front()->clone();
        Opt* tmp=options.front();
        options.pop();
        delete tmp;
    }

    return result;
}

int MsgConfig::getOptionCount() {
    return options.size();
}

MsgIOConfig::MsgIOConfig() : MsgConfig(TYPE_SET_IO_CONF, CONF_IO) { }

MsgIOConfig::~MsgIOConfig() { }

MsgIOConfig* MsgIOConfig::clone() {
    return new MsgIOConfig(*this);
}

MsgImgProcConfig::MsgImgProcConfig() : MsgConfig(TYPE_SET_IMGPROC_CONF, CONF_IMG_PROC) { }

MsgImgProcConfig::~MsgImgProcConfig(){ }

MsgImgProcConfig* MsgImgProcConfig::clone() {
    return new MsgImgProcConfig(*this);
}
