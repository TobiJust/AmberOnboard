/*
 * ModuleSensors.h
 *
 *  Created on: 09.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULESENSORS_H_
#define MODULESENSORS_H_

#include "Module.h"

class ModuleSensors : public Module {
public:
    ModuleSensors();
    virtual ~ModuleSensors();
protected:
    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);
};

#endif /* MODULESENSORS_H_ */
