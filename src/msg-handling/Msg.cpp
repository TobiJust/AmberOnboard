/** \brief      Message instances for inter-module and module-child communication.
 *
 * \details     Classes of type message just hold the data necessary for inter-module
 *              and module-child communication as ValContainer.
 * \author      Daniel Wagenknecht
 * \version     2014-10-31
 * \class       MsgHub
 */

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
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_H, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Z, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Z, shared_ptr<ValString>(new ValString));

    createValue(ARG_OBD_SPEED, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_RPM, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_LOAD, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_COOL_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_AIR_FLOW, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_LVL, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_KM, shared_ptr<ValString>(new ValString));
}

M2M_DataSet::~M2M_DataSet() { }


M2M_Respawn::M2M_Respawn() : Message_M2M(MSG_RESPAWN) {

    createValue(ARG_RESPAWN_CHILD, shared_ptr<ValInt>(new ValInt));
}

M2M_Respawn::~M2M_Respawn() { }

M2M_Event::M2M_Event() : Message_M2M(MSG_EVENT){

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_EVT_TYPE, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
}

M2M_Event::~M2M_Event() { }

M2M_EventDataSet::M2M_EventDataSet() : Message_M2M(MSG_EVENT_INCOMPLETE) {

    cerr << "\033[1;31m M2M_EventDataSet \033[0m: created ("<<this<<")" << endl;

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_H, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Z, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Z, shared_ptr<ValString>(new ValString));

    createValue(ARG_OBD_SPEED, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_RPM, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_LOAD, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_COOL_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_AIR_FLOW, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_LVL, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_KM, shared_ptr<ValString>(new ValString));
}

M2M_EventDataSet::~M2M_EventDataSet() { }

M2M_EventAcquire::M2M_EventAcquire() : Message_M2M(MSG_EVENT_ACQUIRE) { }

M2M_EventAcquire::~M2M_EventAcquire() { }

M2M_Command::M2M_Command() : Message_M2M(MSG_COMMAND) {

    createValue(ARG_COMMAND_TYPE, shared_ptr<ValInt>(new ValInt));
}

M2M_Command::~M2M_Command() {

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
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_H, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Z, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Z, shared_ptr<ValString>(new ValString));

    createValue(ARG_OBD_SPEED, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_RPM, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_LOAD, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_COOL_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_AIR_FLOW, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_LVL, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_KM, shared_ptr<ValString>(new ValString));

}

M2C_DataSet::M2C_DataSet(shared_ptr<M2M_DataSet> data) : Message_M2C(MSG_DATA_COMPLETE){

    // TODO: DELETE

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

M2C_Event::M2C_Event() : Message_M2C(MSG_EVENT){

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_EVT_TYPE, shared_ptr<ValInt>(new ValInt));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Z, shared_ptr<ValString>(new ValString));
}

M2C_Event::~M2C_Event() { }

M2C_EventAcquire::M2C_EventAcquire() : Message_M2C(MSG_EVENT_ACQUIRE) { }

M2C_EventAcquire::~M2C_EventAcquire() {}

M2C_EventDataSet::M2C_EventDataSet() : Message_M2C(MSG_EVENT_INCOMPLETE){

    cerr << "\033[1;31m M2C_EventDataSet \033[0m: created ("<<this<<")" << endl;
    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_N, shared_ptr<ValString>(new ValString));
    createValue(ARG_POS_H, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_ACC_Z, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_X, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Y, shared_ptr<ValString>(new ValString));
    createValue(ARG_GYRO_Z, shared_ptr<ValString>(new ValString));

    createValue(ARG_OBD_SPEED, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_RPM, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_LOAD, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_COOL_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_AIR_FLOW, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_INLET_TEMP, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_LVL, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_FUEL_PRESS, shared_ptr<ValString>(new ValString));
    createValue(ARG_OBD_ENG_KM, shared_ptr<ValString>(new ValString));

}

M2C_EventDataSet::~M2C_EventDataSet() {}

M2C_Respawn::M2C_Respawn() : Message_M2C(MSG_RESPAWN) { }

M2C_Respawn::~M2C_Respawn() { }

M2C_Register::M2C_Register() : Message_M2C(MSG_REGISTER) { }

M2C_Register::~M2C_Register() { }

M2C_Command::M2C_Command() : Message_M2C(MSG_COMMAND) {

    createValue(ARG_COMMAND_TYPE, shared_ptr<ValInt>(new ValInt));
}

M2C_Command::~M2C_Command() { }




















