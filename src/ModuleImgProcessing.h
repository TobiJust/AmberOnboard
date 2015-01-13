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

typedef enum {
    IMG_PROC_OK,
    IMG_PROC_ERR_UNKNOWN,
    IMG_PROC_ERR_INVALID_REFERENCE
} imgProcStatus;

class ModuleImgProcessing : public Module {
public:
    ModuleImgProcessing();
    virtual ~ModuleImgProcessing();

    uint8_t exec_append(string key, shared_ptr<ImgOpExecutor> executor);
    uint8_t exec_delete(string key);
    void exec_clear();

protected:
    unordered_map<string, shared_ptr<ImgOpExecutor>> executors;

    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);
};

#endif /* MODULEIMGPROCESSING_H_ */
