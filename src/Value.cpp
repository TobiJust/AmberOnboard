/*
 * Value.cpp
 *
 *  Created on: 12.11.2014
 *      Author: administrator
 */

#include "Value.h"

Value::Value(int resType, bool initialized) {
    this->resType=resType;
    this->initialized=initialized;
}

Value::~Value() { }

int Value::getType() {
    return this->resType;
}

bool Value::isInitialized() {
    return initialized;
}

// ValDouble class definition.

ValDouble::ValDouble() : Value(VAL_DOUBLE, false) {
    this->value = 0;
}

ValDouble::ValDouble(double value) : Value(VAL_DOUBLE, true) {
    this->value = value;
        this->initialized=true;
}

ValDouble::~ValDouble() { }

double ValDouble::getValue() {
    return value;
}

void ValDouble::setValue(double value) {
    this->value = value;
    this->initialized=true;
}

ValDouble* ValDouble::clone() {
    return new ValDouble(*this);
}



// ValMat class definition.

ValMat::ValMat() : Value(VAL_MAT, false) {
    this->value = NULL;
}

ValMat::ValMat(cv::Mat* value) : Value(VAL_MAT, true) {
    this->value = value;
        this->initialized=true;
}

ValMat::~ValMat() { }

cv::Mat* ValMat::getValue() {
    return value;
}

void ValMat::setValue(cv::Mat* value) {

    if(this->value != NULL)
        delete value;

    this->value = value;

    this->initialized=true;
}

ValMat* ValMat::clone() {
    return new ValMat(*this);
}


// ValInt class definition.

ValInt::ValInt() : Value(VAL_INT, false) {
    this->value = 0;
}

ValInt::ValInt(int value) : Value(VAL_INT, true) {
    this->value = value;
        this->initialized=true;
}

ValInt::~ValInt() { }

int ValInt::getValue() {
    return value;
}

void ValInt::setValue(int value) {
    this->value = value;
    this->initialized=true;
}

ValInt* ValInt::clone() {
    return new ValInt(*this);
}


// ValString class definition.

ValString::ValString() : Value(VAL_STRING, false) {
    this->value = "";
}

ValString::ValString(string value) : Value(VAL_STRING, true) {
    this->value = value;
        this->initialized=true;
}

ValString::~ValString() { }

string ValString::getValue() {
    return value;
}

void ValString::setValue(string value) {
    this->value = value;
    this->initialized=true;
}

ValString* ValString::clone() {
    return new ValString(*this);
}


// ValString class definition.

ValVectorUChar::ValVectorUChar() : Value(VAL_STRING, false) {
    this->value = NULL;
}

ValVectorUChar::ValVectorUChar(vector<unsigned char>* value) : Value(VAL_STRING, true) {
    this->value = value;
        this->initialized=true;
}

ValVectorUChar::~ValVectorUChar() { }

vector<unsigned char>* ValVectorUChar::getValue() {
    return value;
}

void ValVectorUChar::setValue(vector<unsigned char>* value) {
    this->value = value;
    this->initialized=true;
}

ValVectorUChar* ValVectorUChar::clone() {
    return new ValVectorUChar(*this);
}
