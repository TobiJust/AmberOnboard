/*
 * Option.cpp
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "Opt.h"

Opt::Opt(int type, string name, string module) {

    this->oType = type;
    this->name = name;
    this->module = module;
}

Opt::~Opt() { }

int Opt::getType() {
    return this->oType;
}

string Opt::getName() {
    return this->name;
}


// OptDouble class definition.

OptDouble::OptDouble(string module, string name, double value) : Opt(OPT_DOUBLE, name, module) {
    this->value = value;
}

OptDouble::~OptDouble() { }

double OptDouble::getValue() {
    return value;
}

OptDouble* OptDouble::clone(){
    return new OptDouble(*this);
}


// OptShortInt class definition.

OptShortInt::OptShortInt(string module, string name, short value) : Opt(OPT_SHORT_INT, name, module) {
    this->value = value;
}

OptShortInt::~OptShortInt() { }

short OptShortInt::getValue() {
    return value;
}

OptShortInt* OptShortInt::clone(){
    return new OptShortInt(*this);
}


// OptString class definition.

OptString::OptString(string module, string name, string value) : Opt(OPT_STRING, name, module) {
    this->value = value;
}

OptString::~OptString() { }

string OptString::getValue() {
    return value;
}

OptString* OptString::clone(){
    return new OptString(*this);
}
