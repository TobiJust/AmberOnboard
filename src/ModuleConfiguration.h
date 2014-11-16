/*
 * ModuleConfiguration.h
 *
 *  Created on: 07.11.2014
 *      Author: administrator
 */

#ifndef MODULECONFIGURATION_H_
#define MODULECONFIGURATION_H_

#include "message-handling/msg/MsgConfig.h"
#include "Module.h"
#include "Opt.h"

#include <string>
#include <unordered_map>

using namespace std;

class ModuleConfiguration : public Module{
public:
    ModuleConfiguration();
    virtual ~ModuleConfiguration();
    void setValue(int, string, Opt*);
    Opt* getValue(int, string);
protected:
    unordered_map<int, unordered_map<string, Opt*>> map_Module_Opts;

    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);

    void setOption(int, Opt*);
    virtual Msg* pollOptions(int, Msg*);
};

#endif /* MODULECONFIGURATION_H_ */
