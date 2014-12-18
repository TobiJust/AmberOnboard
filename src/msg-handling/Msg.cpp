/*
 * Message.cpp
 *
 *  Created on: 31.10.2014
 *      Author: Daniel Wagenknecht
 */

#include "Msg.h"

#include <iostream>

Msg::Msg(uint8_t type) {



    this->mType = type;
}

Msg::~Msg() {


}

uint8_t Msg::getType() {
    return this->mType;
}

void Msg::setType(uint8_t mType) {
    this->mType = mType;
}


// ------------- MODULE-TO-MODULE COMMUNICATION ------------- //
// ------------- General M2M communication class ------------- //

Message_M2M::Message_M2M(uint8_t type) : Msg(type) { }

Message_M2M::~Message_M2M() { }


// ------------- Terminal input message class ------------- //

M2M_TerminalInput::M2M_TerminalInput() : Message_M2M(MSG_TERM_IN) {

    createValue(ARG_TERM_IN, shared_ptr<ValString>(new ValString));
}

M2M_TerminalInput::~M2M_TerminalInput() { }


// ------------- Terminate broadcast message class ------------- //

M2M_TermBroad::M2M_TermBroad() : Message_M2M(MSG_TERM_BROADCAST) { }

M2M_TermBroad::~M2M_TermBroad() { }


// ------------- Data acquired message class ------------- //

M2M_DataAcquired::M2M_DataAcquired() : Message_M2M(MSG_DATA_ACQUIRED) { }

M2M_DataAcquired::~M2M_DataAcquired() { }


// ------------- Data set message class ------------- //

M2M_DataSet::M2M_DataSet() : Message_M2M(MSG_DATA_INCOMPLETE){

    cerr << "\033[1;31m M2M_DataSet \033[0m: created ("<<this<<")" << endl;

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_N, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_X, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Y, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Z, shared_ptr<ValVectorUChar>(new ValVectorUChar));
}

M2M_DataSet::~M2M_DataSet() {
    cerr << "\033[1;31m M2M_DataSet \033[0m: deleted ("<<this<<")" << endl;
}


// ------------- MODULE-TO-CHILD COMMUNICATION ------------- //
// ------------- General M2C communication class ------------- //

Message_M2C::Message_M2C(uint8_t type) : Msg(type) { }

Message_M2C::~Message_M2C() { }


// ------------- Terminal input message class ------------- //

M2C_TerminalInput::M2C_TerminalInput() : Message_M2C(MSG_TERM_IN) {

    createValue(ARG_TERM_IN, shared_ptr<ValString>(new ValString));
}

M2C_TerminalInput::~M2C_TerminalInput() { }


// ------------- Terminal output message class ------------- //

M2C_TerminalOutput::M2C_TerminalOutput() : Message_M2C(MSG_TERM_IN) {

    createValue(ARG_TERM_OUT, shared_ptr<ValString>(new ValString));
}

M2C_TerminalOutput::~M2C_TerminalOutput() { }


// ------------- Data acquired message class ------------- //

M2C_DataAcquired::M2C_DataAcquired() : Message_M2C(MSG_DATA_ACQUIRED) { }

M2C_DataAcquired::~M2C_DataAcquired() { }


// ------------- Data set message class ------------- //

M2C_DataSet::M2C_DataSet() : Message_M2C(MSG_DATA_INCOMPLETE){

    cerr << "\033[1;31m M2C_DataSet \033[0m: created ("<<this<<")" << endl;

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_N, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_X, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Y, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Z, shared_ptr<ValVectorUChar>(new ValVectorUChar));
}

M2C_DataSet::M2C_DataSet(shared_ptr<M2M_DataSet> data) : Message_M2C(MSG_DATA_COMPLETE){

    cerr << "\033[1;31m M2C_DataSet \033[0m: created ("<<this<<")" << endl;

    shared_ptr<Value> image, posN, posE, accX, accY, accZ;
    data->getValue(ARG_IMG, image);
    data->getValue(ARG_POS_N, posN);
    data->getValue(ARG_POS_E, posE);
    data->getValue(ARG_ACC_X, accX);
    data->getValue(ARG_ACC_Y, accY);
    data->getValue(ARG_ACC_Z, accZ);

    createValue(ARG_IMG, image);
    createValue(ARG_POS_N, posN);
    createValue(ARG_POS_E, posE);
    createValue(ARG_ACC_X, accX);
    createValue(ARG_ACC_Y, accY);
    createValue(ARG_ACC_Z, accZ);
}

M2C_DataSet::~M2C_DataSet() {
    cerr << "\033[1;31m M2C_DataSet \033[0m: deleted ("<<this<<")" << endl;

}






















