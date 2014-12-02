/*
 * ValContainer.cpp
 *
 *  Created on: 14.11.2014
 *      Author: administrator
 */

#include "ValContainer.h"

#include <iostream>

int ValContainer::count = 0;
mutex ValContainer::countLock;

ValContainer::ValContainer() {

    /*
    {
        countLock.lock();
        cerr << "\033[1;32mValContainer\033[0m: (" << this << ") count now " << ++count  << " ( + created )" << endl;
        countLock.unlock();
    }
     */
}

ValContainer::~ValContainer() {

    /*
    {
        countLock.lock();
        cerr << "\033[1;31mValContainer\033[0m: (" << this << ") count now " << --count << " ( # deleted )" << endl;
        countLock.unlock();
    }
     */
}

void ValContainer::createValue(string name, shared_ptr<Value> value) {

    config.insert(make_pair(name, value));
}

void ValContainer::deleteKey(string name) {
    config.erase(name);
}

uint8_t ValContainer::setValue(string name, const shared_ptr<Value> &val) {

    // cerr << "\033[1;33mValContainer\033[0m: (" << this << ") setting new value " << name << endl;

    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Type of argument does not match option type.
    if(val->getType() != valIt->second->getType())
        return ERR_TYPE_MISMATCH;



    // Switch Value type for setting appropriate data structure.
    switch (val->getType()) {
    case VAL_DOUBLE:
    {
        // Create and initialize child pointer.
        shared_ptr<ValDouble> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValDouble>(val);
        oldValue = dynamic_pointer_cast<ValDouble>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_MAT:
    {
        // Create and initialize child pointer.
        shared_ptr<ValMat> newValue, oldValue;

        newValue = dynamic_pointer_cast<ValMat>(val);
        oldValue = dynamic_pointer_cast<ValMat>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_INT:
    {
        // Create and initialize child pointer.
        shared_ptr<ValInt> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValInt>(val);
        oldValue = dynamic_pointer_cast<ValInt>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_STRING:
    {

        // Create and initialize child pointer.
        shared_ptr<ValString> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValString>(val);
        oldValue = dynamic_pointer_cast<ValString>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_UCHAR_VECTOR:
    {
        // Create and initialize child pointer.
        shared_ptr<ValVectorUChar> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValVectorUChar>(val);
        oldValue = dynamic_pointer_cast<ValVectorUChar>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    default:
        return ERR_UNKNOWN;
    }

    // cerr << "\033[1;32mValContainer\033[0m: successfull" << endl;

    return OK;

}

uint8_t ValContainer::getValue(string name, shared_ptr<Value> &val) {

    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Value is not initialized
    if(!valIt->second->isInitialized())
        return ERR_UNSET_VALUE;

    // Set Value.
    val=valIt->second;

    return OK;

}

bool ValContainer::initialized() {

    // Check if all config values are initialized.
    for(auto it : config)
        if(!it.second->isInitialized()){
            // cerr << "ValContainer: value " << it.first << " uninited" << endl;
            return false;
        }

    return true;

}

uint8_t ValContainer::getValueCount() {

    return config.size();

}
