/*
 * ImgOpExecutor.h
 *
 *  Created on: 18.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IMGOPEXECUTOR_H_
#define IMGOPEXECUTOR_H_

#define MAX_CAPTURES    0xFF
#define ARG_THUMB   "Thumbnail"

#include "ImgCapture.h"
#include "ImgOperator.h"
#include "../Child.h"

#include <opencv2/highgui/highgui.hpp>

#include <mutex>
#include <vector>

using namespace std;

typedef enum {
    EXEC_OK,
    EXEC_INVALID_REFERENCE,
    EXEC_OUT_OF_BOUNDS
}execReturns;


class ResultContainer : public ValContainer {
public:
    ResultContainer();
    virtual ~ResultContainer();
};

class ImgOpExecutor : public Child {
public:

    ImgOpExecutor();
    ImgOpExecutor(shared_ptr<ImgCapture> &capture);
    virtual ~ImgOpExecutor();

    uint8_t setResult(string identifier, shared_ptr<Value> &target);
    uint8_t getResult(string identifier, shared_ptr<Value> &target);
    uint8_t setValue(string identifier, shared_ptr<Value> target);

    uint8_t cap_append(shared_ptr<ImgCapture> capture);
    uint8_t cap_delete(shared_ptr<ImgCapture> capture);
    uint8_t cap_delete(uint8_t index);
    void cap_clear();
    uint8_t cap_swap(uint8_t index1, uint8_t index2);
    uint8_t cap_firstIndexOf(uint8_t captureID);

    uint8_t op_append(shared_ptr<ImgOperator> op);
    uint8_t op_delete(shared_ptr<ImgOperator> op);
    uint8_t op_delete(uint8_t index);
    void op_clear();
    uint8_t op_swap(uint8_t index1, uint8_t index2);
    uint8_t op_firstIndexOf(uint8_t opType);

    uint8_t execute();
    int run();

protected:
    vector<shared_ptr<ImgOperator>> imageOperators;
    vector<shared_ptr<ImgCapture>> imageCaptures;
    ResultContainer results;
    mutex producer, captureMutex;






    /*
    vector<bool> responsable;
    vector<ImgCapture*> captures;
    unordered_map<int,  shared_ptr<Value>> currentResults;
    shared_ptr<ImgCapture> capInstance;
    vector<shared_ptr<ImgOperator>> operators;
     */
};


#endif /* IMGOPEXECUTOR_H_ */
