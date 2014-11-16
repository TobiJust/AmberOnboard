/*
 * ModuleSensors.h
 *
 *  Created on: 09.11.2014
 *      Author: administrator
 */

#ifndef MODULESENSORS_H_
#define MODULESENSORS_H_

#include "Module.h"

class ModuleSensors : public Module {
public:
    ModuleSensors();
    virtual ~ModuleSensors();
protected:
    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);
};

#endif /* MODULESENSORS_H_ */
