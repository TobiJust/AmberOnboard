/*
 * OpComposite.h
 *
 *  Created on: 28.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef OPCOMPOSITE_H_
#define OPCOMPOSITE_H_

#include "Container.h"
#include "ImgOperator.h"

#include <vector>

class OpComposite : public ImgOperator {
public:
    OpComposite(uint8_t type);
    virtual ~OpComposite()=0;

    uint8_t op_append(shared_ptr<ImgOperator> &op);
    uint8_t op_delete(shared_ptr<ImgOperator> &op);
    uint8_t op_delete(uint8_t index);
    void op_clear();
    uint8_t op_swap(uint8_t index1, uint8_t index2);
    uint8_t op_firstIndexOf(uint8_t opType);

    void connect(string resultName, string paramName);

    virtual uint8_t setValue(string name, const shared_ptr<Value> &val);
    virtual uint8_t getValue(string name, shared_ptr<Value> &val);
    virtual bool initialized();
    virtual void createCaptures(uint8_t captureCount);
    virtual uint8_t getCaptureCount();
private:
    vector<shared_ptr<ImgOperator>> imageOperators;
    unordered_map<string,string> connections;
    virtual uint8_t process(unordered_map<string,shared_ptr<Value>> &results);
};

#endif /* OPCOMPOSITE_H_ */
