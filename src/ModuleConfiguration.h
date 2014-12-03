/*
 * ModuleConfiguration.h
 *
 *  Created on: 07.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULECONFIGURATION_H_
#define MODULECONFIGURATION_H_

#include "msg-handling/MsgConfig.h"
#include "Module.h"
#include "Opt.h"

#include <string>
#include <unordered_map>

using namespace std;

class ModuleConfiguration : public Module{
public:
    ModuleConfiguration();
    virtual ~ModuleConfiguration();
    void setValue(uint8_t, string, Opt*);
    Opt* getValue(uint8_t, string);
protected:
    unordered_map<uint8_t, unordered_map<string, Opt*>> map_Module_Opts;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);

    void setOption(uint8_t, Opt*);
    virtual Msg* pollOptions(uint8_t, Msg*);
};

#endif /* MODULECONFIGURATION_H_ */
