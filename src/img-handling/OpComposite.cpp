/*
 * OpComposite.cpp
 *
 *  Created on: 28.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "OpComposite.h"

OpComposite::OpComposite(uint8_t type) : ImgOperator(type, 1) {}

OpComposite::~OpComposite() { }

void OpComposite::connect(string resultName, string paramName) {

    auto connIt = this->connections.find(resultName);

    if(connIt == this->connections.end())
        this->connections.insert(make_pair(resultName, paramName));
    else
        connIt->second = paramName;

}

void OpComposite::createCaptures(uint8_t captureCount) { }

uint8_t OpComposite::setValue(string name, const shared_ptr<Value> &val) {

    uint8_t result=0;

     for (auto leafIt : this->imageOperators) {

        // Try setting value.
        uint8_t tmpResult = leafIt->setValue(name,val);

        // Value matched, return.
        if (tmpResult != ERR_NO_SUCH_KEY && tmpResult != OK)
            return tmpResult;
        else if (tmpResult == OK)
            result = tmpResult;
    }

    return result;
}

uint8_t OpComposite::getValue(string name, shared_ptr<Value> &val) {

    uint8_t result=0;

    for (auto leafIt : this->imageOperators) {

        // Try getting value.
        result = leafIt->getValue(name,val);

        // Value matched or error, return.
        if (result!=ERR_NO_SUCH_KEY)
            return result;
    }

    return ERR_NO_SUCH_KEY;
}

bool OpComposite::initialized() {

    for (auto leafIt : this->imageOperators) {

        // Value matched, return.
        if (!leafIt->initialized())
            return false;
    }

    return true;
}

uint8_t OpComposite::getCaptureCount() {

    uint8_t result = 0;

    for (auto leafIt : this->imageOperators) {

        // Value matched, return.
        if (leafIt->getCaptureCount() > result)
            result = leafIt->getCaptureCount();
    }

    return result;
}

uint8_t OpComposite::process(unordered_map<string,shared_ptr<Value>> &results) {

    // cerr << "OpComposite: got called" << endl;

    uint8_t status = 0;

    auto leafIt = this->imageOperators.begin();

    while (leafIt != this->imageOperators.end()) {

        // Temp variable.
        unordered_map<string,shared_ptr<Value>> tmp;

        // Apply operator.
        status = (*leafIt)->apply(tmp);

        // An error occurred.
        if (status != OK)
            return status;

        // There are more operators pending, so try setting their parameters.
        if ((leafIt+1) != this->imageOperators.end())

            // Iterate over results and set parameters of successor.
            for (auto connIt : this->connections) {

                auto resIt = tmp.find(connIt.first);
                shared_ptr<Value> dasdsadsa = (resIt->second);
                // cerr << "OpComposite: setting " << connIt.first << " to " << dasdsadsa << endl;
/*
                cv::imshow("OpComposite", *(dynamic_pointer_cast<ValMat>(resIt->second)->getValue())); //display road image
                if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
                {
                    cout << "esc key is pressed by user" << endl;
                    throw 20;
                }
*/
                if(resIt != tmp.end())
                    (*(leafIt+1))->setValue(connIt.second, resIt->second);
            }

        // Insert results to list.
        results.insert(tmp.begin(), tmp.end());

        leafIt++;
    }

    return status;

}

uint8_t OpComposite::op_append(shared_ptr<ImgOperator> &op) {

    // Last of the 256 possible indices is reserved.
    if(this->imageOperators.size() < 0xFF) {

        if (op) {
            this->imageOperators.push_back(op);
            return COMP_OK;
        }

        return COMP_INVALID_REFERENCE;
    }

    return COMP_OUT_OF_BOUNDS;
}

uint8_t OpComposite::op_delete(shared_ptr<ImgOperator> &op) {

    // Pointer must not point to NULL.
    if (op) {

        // Get iterator for operator instances.
        auto delIt = this->imageOperators.begin();

        // Iterate list and delete operator, if found.
        while (delIt != this->imageOperators.end()) {

            if (*delIt == op) {
                this->imageOperators.erase(delIt);
                return COMP_OK;
            }

            delIt++;
        }
    }

    return COMP_INVALID_REFERENCE;
}

uint8_t OpComposite::op_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageOperators.size()) {

        // Erase from list.
        this->imageOperators.erase(this->imageOperators.begin()+index);
        return COMP_OK;
    }

    return COMP_OUT_OF_BOUNDS;
}

void OpComposite::op_clear() {

    // Clear complete list.
    this->imageOperators.clear();
}

uint8_t OpComposite::op_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageOperators.size() && index2 < this->imageOperators.size()) {

        shared_ptr<ImgOperator> ptr1 = this->imageOperators[index1];
        shared_ptr<ImgOperator> ptr2 = this->imageOperators[index2];
        ptr1.swap(ptr2);

        return COMP_OK;
    }
    return COMP_OUT_OF_BOUNDS;
}

uint8_t OpComposite::op_firstIndexOf(uint8_t opType) {

    // Get iterator for operator instances.
    auto delIt = this->imageOperators.begin();

    // Iterate list and delete operator, if found.
    while (delIt != this->imageOperators.end()) {

        // Found capture id.
        if ((*delIt)->getType() == opType)
            return delIt - this->imageOperators.begin();

        delIt++;
    }

    // Not found.
    return 0xFF;
}




























