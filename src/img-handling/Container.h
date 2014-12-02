/*
 * ImgOpContainer.h
 *
 *  Created on: 28.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef IMGOPCONTAINER_H_
#define IMGOPCONTAINER_H_

#include "ImgOperator.h"
#include "ImgCapture.h"

typedef enum {
    COMP_OK,
    COMP_INVALID_REFERENCE,
    COMP_OUT_OF_BOUNDS
}opReturns;

class ImgOpContainer {
public:
    ImgOpContainer();
    virtual ~ImgOpContainer()=0;

    uint8_t op_append(shared_ptr<ImgOperator> &op);
    uint8_t op_delete(shared_ptr<ImgOperator> &op);
    uint8_t op_delete(uint8_t index);
    void op_clear();
    uint8_t op_swap(uint8_t index1, uint8_t index2);
    uint8_t op_firstIndexOf(uint8_t opType);

    uint8_t op_getSize();
    vector<shared_ptr<ImgOperator>>::iterator getBeginElement();
    vector<shared_ptr<ImgOperator>>::iterator getEndElement();
private:
    vector<shared_ptr<ImgOperator>> imageOperators;
};

class CapContainer {
public:
    CapContainer();
    virtual ~CapContainer()=0;

    uint8_t cap_append(shared_ptr<ImgCapture> &capture);
    uint8_t cap_delete(shared_ptr<ImgCapture> &capture);
    uint8_t cap_delete(uint8_t index);
    void cap_clear();
    uint8_t cap_swap(uint8_t index1, uint8_t index2);
    uint8_t cap_firstIndexOf(uint8_t captureID);
    shared_ptr<ImgCapture> cap_get(uint8_t index);

    uint8_t cap_getSize();
    vector<shared_ptr<ImgCapture>>::iterator cap_getBeginElement();
    vector<shared_ptr<ImgCapture>>::iterator cap_getEndElement();
private:
    vector<shared_ptr<ImgCapture>> imageCaptures;
};

#endif /* IMGOPCONTAINER_H_ */
