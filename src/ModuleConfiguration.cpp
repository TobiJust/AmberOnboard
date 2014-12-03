/*
 * ModuleConfiguration.cpp
 *
 *  Created on: 07.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ModuleConfiguration.h"

ModuleConfiguration::ModuleConfiguration() {

    // Register for message types.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_SET_IMGPROC_CONF);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_SET_IO_CONF);

}

ModuleConfiguration::~ModuleConfiguration() {
    // TODO Auto-generated destructor stub
}

void ModuleConfiguration::setValue(uint8_t confGroup, string name, Opt* option) {

    // Get pair iterator for module name.
    auto modIt = map_Module_Opts.find(confGroup);

    // Key found in map.
    if(modIt != map_Module_Opts.end()) {

        // Get pair iterator for option name.
        auto optIt = (*modIt).second.find(name);

        // Key found in map.
        if(optIt != (*modIt).second.end()) {

            // Set new option and delete old one.
            Opt* old = (*optIt).second;
            (*optIt).second = option;
            delete old;

        }else // Insert new Option.
            (*modIt).second.insert(make_pair(name, option));

    } else {

        // Insert new module with option list and new option.
        unordered_map<string, Opt*> inner_map;
        inner_map.insert(make_pair(name, option));
        map_Module_Opts.insert(make_pair(confGroup, inner_map));
    }
}

Opt* ModuleConfiguration::getValue(uint8_t confGroup, string name) {

    // Return value.
    Opt* result = NULL;

    // Get pair iterator for module name.
    auto modIt = map_Module_Opts.find(confGroup);

    // Key found in map.
    if(modIt != map_Module_Opts.end()) {

        // Get pair iterator for option name.
        auto optIt = (*modIt).second.find(name);

        // Key found in map.
        if(optIt != (*modIt).second.end())

            // Set new option and delete old one.
            result = (*optIt).second;

    }

    return result;
}

void ModuleConfiguration::setOption(uint8_t confGroup, Opt* opt) {

    switch (opt->getType()) {
    case OPT_DOUBLE:
        OptDouble* dOpt;
        dOpt = dynamic_cast<OptDouble*>(opt);
        this->setValue(confGroup, dOpt->getName(), dOpt);
        break;
    case OPT_SHORT_INT:
        OptShortInt* siOpt;
        siOpt = dynamic_cast<OptShortInt*>(opt);
        this->setValue(confGroup, siOpt->getName(), siOpt);
        break;
    case OPT_STRING:
        OptString* sOpt;
        sOpt = dynamic_cast<OptString*>(opt);
        this->setValue(confGroup, sOpt->getName(), sOpt);
        break;
    default:
        break;
    }

}

Msg* ModuleConfiguration::pollOptions(uint8_t confGroup, Msg* msg) {

    /*
    // Since answer contains the exact same options,
    // the origin message could simple be copied in first place.
    Msg* answer=msg->clone();

    // Get polymorph subclass pointer
    MsgConfig* imgProc_Instance;
    imgProc_Instance = dynamic_cast<MsgConfig*>(msg);

    // Iterate over all pending options of the message.
    while (imgProc_Instance->getOptionCount())

        // Each option gets set in the options map.
        setOption(confGroup, imgProc_Instance->getNextOption());

    return answer;
    */

    return NULL;

}

uint8_t ModuleConfiguration::countMsgFromChildren() {
    return 0;
}

uint8_t ModuleConfiguration::pollMsgFromChildren() {
    return 0;
}

Msg* ModuleConfiguration::processMsg(Msg* msg) {

    Msg* result=NULL;
/*
    switch (msg->getType()) {
    case MSG_SET_IMGPROC_CONF:

        result=pollOptions(CONF_IMG_PROC, msg);

        // Set new message type to actually cause configuration.
        result->setType(MSG_DO_IMGPROC_CONF);
        break;

    case MSG_SET_IO_CONF:

        result=pollOptions(CONF_IO, msg);

        // Set new message type to actually cause configuration.
        result->setType(MSG_DO_IO_CONF);
        break;

    default:
        break;
    }
*/

    return result;
}
