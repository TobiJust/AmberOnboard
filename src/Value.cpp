/*
 * Value.cpp
 *
 *  Created on: 12.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "Value.h"

#include <iostream>


int Value::count = 0;
mutex Value::countLock;

Value::Value(int resType, bool initialized) {
    {
        countLock.lock();
        // cerr << "Value: count now " << ++count << " ( + created as type " << resType << " )" << endl;
        countLock.unlock();
    }
    this->resType=resType;
    this->initialized=initialized;
}

Value::~Value() {
    {
        countLock.lock();
        // cerr << "Value: count now " << --count << " ( # deleted as type " << resType << " )" << endl;
        countLock.unlock();
    }
}

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
}

ValDouble::~ValDouble() { }

double ValDouble::getValue() {
    return value;
}

void ValDouble::setValue(double value) {
    this->value = value;
    this->initialized=true;
}

shared_ptr<Value> ValDouble::clone() {
    return static_pointer_cast<ValDouble, Value>(actualClone());
}

shared_ptr<ValDouble> ValDouble::actualClone() {
    return shared_ptr<ValDouble>(new ValDouble(*this));
}



// ValMat class definition.

ValMat::ValMat() : Value(VAL_MAT, false) {
    this->value = NULL;
}

ValMat::ValMat(shared_ptr<cv::Mat> &value) : Value(VAL_MAT, true) {
    this->value = value;
}

ValMat::~ValMat() { }

shared_ptr<cv::Mat> ValMat::getValue() {
    return value;
}

void ValMat::setValue(shared_ptr<cv::Mat> value) {

    this->value = value;
    this->initialized=true;
}

shared_ptr<Value> ValMat::clone() {
    return static_pointer_cast<ValMat, Value>(actualClone());
}

shared_ptr<ValMat> ValMat::actualClone() {
    return shared_ptr<ValMat>(new ValMat(*this));
}

// ValInt class definition.

ValInt::ValInt() : Value(VAL_INT, false) {
    this->value = 0;
}

ValInt::ValInt(int value) : Value(VAL_INT, true) {
    this->value = value;
}

ValInt::~ValInt() { }

int ValInt::getValue() {
    return value;
}

void ValInt::setValue(int value) {
    this->value = value;
    this->initialized=true;
}

shared_ptr<Value> ValInt::clone() {
    return static_pointer_cast<ValInt, Value>(actualClone());
}

shared_ptr<ValInt> ValInt::actualClone() {
    return shared_ptr<ValInt>(new ValInt(*this));
}


// ValString class definition.

ValString::ValString() : Value(VAL_STRING, false) {
    this->value = "";
}

ValString::ValString(string value) : Value(VAL_STRING, true) {
    this->value = value;
}

ValString::~ValString() { }

string ValString::getValue() {
    return value;
}

void ValString::setValue(string value) {
    this->value = value;
    this->initialized=true;
}

shared_ptr<Value> ValString::clone() {
    return static_pointer_cast<ValString, Value>(actualClone());
}

shared_ptr<ValString> ValString::actualClone() {
    return shared_ptr<ValString>(new ValString(*this));
}


// ValString class definition.

ValVectorUChar::ValVectorUChar() : Value(VAL_UCHAR_VECTOR, false) {
    this->value = NULL;
}

ValVectorUChar::ValVectorUChar(shared_ptr<vector<unsigned char>> value) : Value(VAL_UCHAR_VECTOR, true) {
    this->value = value;
}

ValVectorUChar::~ValVectorUChar() {
}

shared_ptr<vector<unsigned char>> ValVectorUChar::getValue() {
    return value;
}

void ValVectorUChar::setValue(shared_ptr<vector<unsigned char>> value) {
    this->value = value;
    this->initialized=true;
}

shared_ptr<Value> ValVectorUChar::clone() {
    return static_pointer_cast<ValVectorUChar, Value>(actualClone());
}

shared_ptr<ValVectorUChar> ValVectorUChar::actualClone() {
    return shared_ptr<ValVectorUChar>(new ValVectorUChar(*this));
}
