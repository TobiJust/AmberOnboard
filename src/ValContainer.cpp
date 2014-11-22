/*
 * ValContainer.cpp
 *
 *  Created on: 14.11.2014
 *      Author: administrator
 */

#include "ValContainer.h"

#include <iostream>

ValContainer::ValContainer() {
    // TODO Auto-generated constructor stub

}

ValContainer::~ValContainer() {
    // TODO Auto-generated destructor stub
}

void ValContainer::createValue(string name, Value* value) {
    config.insert(make_pair(name, value));
}

int ValContainer::setValue(string name, Value* val) {


    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Type of argument does not match option type.
    if(val->getType() != valIt->second->getType())
        return ERR_TYPE_MISMATCH;

    // Switch Value type for setting appropriate data structure.
    switch (valIt->second->getType()) {
    case VAL_DOUBLE:

        // Create and initialize child pointer.
        ValDouble *newDouble, *oldDouble;
        newDouble = dynamic_cast<ValDouble*>(val);
        oldDouble = dynamic_cast<ValDouble*>(valIt->second);

        // Set new value.
        oldDouble->setValue(newDouble->getValue());

        break;

    case VAL_MAT:

        // Create and initialize child pointer.
        ValMat *newMat, *oldMat;
        newMat = dynamic_cast<ValMat*>(val);
        oldMat = dynamic_cast<ValMat*>(valIt->second);

        // Set new value.
        oldMat->setValue(newMat->getValue());

        break;

    case VAL_INT:

        // Create and initialize child pointer.
        ValInt *newInt, *oldInt;
        newInt = dynamic_cast<ValInt*>(val);
        oldInt = dynamic_cast<ValInt*>(valIt->second);

        // Set new value.
        oldInt->setValue(newInt->getValue());

        break;

    case VAL_STRING:

            // Create and initialize child pointer.
            ValString *newString, *oldString;
            newString = dynamic_cast<ValString*>(val);
            oldString = dynamic_cast<ValString*>(valIt->second);

            // Set new value.
            oldString->setValue(newString->getValue());

            break;

    case VAL_UCHAR_VECTOR:

            // Create and initialize child pointer.
            ValVectorUChar *newVector, *oldVector;
            newVector = dynamic_cast<ValVectorUChar*>(val);
            oldVector = dynamic_cast<ValVectorUChar*>(valIt->second);

            // Set new value.
            oldVector->setValue(newVector->getValue());

            break;

    default:
        return ERR_UNKNOWN;
    }

    return OK;

}

int ValContainer::getValue(string name, Value** val) {

    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Value is not initialized
    if(!valIt->second->isInitialized())
        return ERR_UNSET_VALUE;

    // Set Value.
    *val = valIt->second->clone();

    return OK;

}

bool ValContainer::initialized() {

    // Check if all config values are initialized.
    for(auto it : config)
        if(!it.second->isInitialized())
            return false;

    return true;

}
