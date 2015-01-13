/*
 * ModuleIO.h
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULEIO_H_
#define MODULEIO_H_

#include "Module.h"
#include "io-handling/Config.h"
#include "io-handling/SensorIO.h"

#include <iostream>

using namespace std;

class ModuleIO : public Module {
public:
    ModuleIO();
    virtual ~ModuleIO();
    void setSensorIO(shared_ptr<SensorIO> sensors);

protected:

    // Config conf;
    shared_ptr<SensorIO> sensors;

    shared_ptr<ValString> createValue(unordered_map<string, string> &data, string string);

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULEIO_H_ */
