/*
 * ModuleImgProcessing.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef MODULEIMGPROCESSING_H_
#define MODULEIMGPROCESSING_H_

#include "Module.h"

#include <unordered_set>

class ModuleImgProcessing : public Module {
public:
    ModuleImgProcessing();
    virtual ~ModuleImgProcessing();
protected:

    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual Msg* processMsg(Msg*);
};

#endif /* MODULEIMGPROCESSING_H_ */
