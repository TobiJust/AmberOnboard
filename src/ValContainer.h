/*
 * ValContainer.h
 *
 *  Created on: 14.11.2014
 *      Author: administrator
 */

#ifndef VALCONTAINER_H_
#define VALCONTAINER_H_

#include "Value.h"

#include <string>
#include <unordered_map>

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
    int setValue(string name, Value* val);
    int getValue(string name, Value** val);
    bool initialized();
protected:
    unordered_map<string,Value*> config;
};

#endif /* VALCONTAINER_H_ */
