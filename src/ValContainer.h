/*
 * ValContainer.h
 *
 *  Created on: 14.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef VALCONTAINER_H_
#define VALCONTAINER_H_

#include "Value.h"

#include <string>
#include <unordered_map>

#include <memory>
#include <mutex>

typedef enum {
    OK,
    ERR_UNKNOWN,
    ERR_NO_SUCH_KEY,
    ERR_TYPE_MISMATCH,
    ERR_UNSET_VALUE
}status;

using namespace std;

class ValContainer {
public:
    ValContainer();
    virtual ~ValContainer()=0;
    virtual void createValue(string name, shared_ptr<Value> val);
    void deleteKey(string name);
    virtual uint8_t setValue(string name, const shared_ptr<Value> &val);
    virtual uint8_t getValue(string name, shared_ptr<Value> &val);
    virtual bool initialized();
    uint8_t getValueCount();
protected:
    unordered_map<string,shared_ptr<Value>> config;
    static int count;
    static mutex countLock;
};

#endif /* VALCONTAINER_H_ */
