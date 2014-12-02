/*
 * ModuleImgProcessing.h
 *
 *  Created on: 08.11.2014
 *      Author: administrator
 */

#ifndef MODULEIMGPROCESSING_H_
#define MODULEIMGPROCESSING_H_

#define OUTER_CAMERA    "Front"

#include "Module.h"
#include "img-handling/OpPrepare.h"
#include "img-handling/ImgOpExecutor.h"

#include <string>
#include <unordered_map>

class ModuleImgProcessing : public Module {
public:
    static    CamCapture* cam;

    ModuleImgProcessing();
    virtual ~ModuleImgProcessing();
protected:
    unordered_map<string, shared_ptr<ImgOpExecutor>> executors;

    virtual int countMsgFromChildren();
    virtual int pollMsgFromChildren();
    virtual shared_ptr<Msg> processMsg(shared_ptr<Msg>);
};

#endif /* MODULEIMGPROCESSING_H_ */
