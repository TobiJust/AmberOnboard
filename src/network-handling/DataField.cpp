/*
 * DataField.cpp
 *
 *  Created on: 23.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "DataField.h"

#include <iostream>

int DataField::count = 0;
mutex DataField::countLock;

DataField::DataField(int type) {
    {
        countLock.lock();
        // cerr << "DataField: (" << this << ") count now " << ++count << " ( + created as type " << type << " )" << endl;
        countLock.unlock();
    }
    this->dataType=type;
}

DataField::~DataField() {
    {
            countLock.lock();
            // cerr << "DataField: (" << this << ") count now " << --count << " ( # deleted as type " << this->dataType << " )" << endl;
            countLock.unlock();
        }


    this->config.clear();
}

void DataField::setType(int type) {
    this->dataType=type;
}

int DataField::getType() {
    return this->dataType;
}

AcquiredData_In::AcquiredData_In() : DataField(DATA_ACQUIRED) {

    createValue(ARG_ACQUIRED_DATA, shared_ptr<ValInt>(new ValInt));

}

AcquiredData_In::~AcquiredData_In() { }

AcquiredData_Out::AcquiredData_Out() : DataField(DATA_ACQUIRED) {

    createValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_E, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_POS_N, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_X, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Y, shared_ptr<ValVectorUChar>(new ValVectorUChar));
    createValue(ARG_ACC_Z, shared_ptr<ValVectorUChar>(new ValVectorUChar));

}

AcquiredData_Out::~AcquiredData_Out() { }
