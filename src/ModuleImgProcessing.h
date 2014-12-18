/*
 * ModuleImgProcessing.h
 *
 *  Created on: 08.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MODULEIMGPROCESSING_H_
#define MODULEIMGPROCESSING_H_

#define PREPARE    "Prepare"

#include "Module.h"
#include "img-handling/OpPrepare.h"
#include "img-handling/ImgOpExecutor.h"

#include <string>
#include <unordered_map>

class ModuleImgProcessing : public Module {
public:
 //    static    CamCapture* cam;

    ModuleImgProcessing();
    virtual ~ModuleImgProcessing();
protected:
    unordered_map<string, shared_ptr<ImgOpExecutor>> executors;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULEIMGPROCESSING_H_ */
