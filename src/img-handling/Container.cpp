/*
 * ImgOpContainer.cpp
 *
 *  Created on: 28.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "Container.h"

ImgOpContainer::ImgOpContainer() {
    // TODO Auto-generated constructor stub

}

ImgOpContainer::~ImgOpContainer() {
    // TODO Auto-generated destructor stub
}


uint8_t ImgOpContainer::op_append(shared_ptr<ImgOperator> &op) {

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

uint8_t ImgOpContainer::op_delete(shared_ptr<ImgOperator> &op) {

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

uint8_t ImgOpContainer::op_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageOperators.size()) {

        // Erase from list.
        this->imageOperators.erase(this->imageOperators.begin()+index);
        return COMP_OK;
    }

    return COMP_OUT_OF_BOUNDS;
}

void ImgOpContainer::op_clear() {

    // Clear complete list.
    this->imageOperators.clear();
}

uint8_t ImgOpContainer::op_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageOperators.size() && index2 < this->imageOperators.size()) {

        shared_ptr<ImgOperator> ptr1 = this->imageOperators[index1];
        shared_ptr<ImgOperator> ptr2 = this->imageOperators[index2];
        ptr1.swap(ptr2);

        return COMP_OK;
    }
    return COMP_OUT_OF_BOUNDS;
}

uint8_t ImgOpContainer::op_firstIndexOf(uint8_t opType) {

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

uint8_t ImgOpContainer::op_getSize() {

    return this->imageOperators.size();
}

vector<shared_ptr<ImgOperator>>::iterator ImgOpContainer::getBeginElement() {
    return this->imageOperators.begin();
}

vector<shared_ptr<ImgOperator>>::iterator ImgOpContainer::getEndElement() {
    return this->imageOperators.end();
}


CapContainer::CapContainer() {
    // TODO Auto-generated constructor stub

}

CapContainer::~CapContainer() {
    // TODO Auto-generated destructor stub
}

uint8_t CapContainer::cap_append(shared_ptr<ImgCapture> &capture) {

    // Last of the 256 possible indices is reserved.
    if(this->imageCaptures.size() < 0xFF) {

        if (capture) {
            this->imageCaptures.push_back(capture);
            return COMP_OK;
        }

        return COMP_INVALID_REFERENCE;
    }

    return COMP_OUT_OF_BOUNDS;
}

uint8_t CapContainer::cap_delete(shared_ptr<ImgCapture> &capture) {

    // Pointer must not point to NULL.
    if (capture) {

        // Get iterator for capture instances.
        auto delIt = this->imageCaptures.begin();

        // Iterate list and delete capture, if found.
        while (delIt != this->imageCaptures.end()) {

            if (*delIt == capture) {
                this->imageCaptures.erase(delIt);
                return COMP_OK;
            }

            delIt++;
        }
    }

    return COMP_INVALID_REFERENCE;
}

uint8_t CapContainer::cap_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageCaptures.size()) {

        // Erase from list.
        this->imageCaptures.erase(this->imageCaptures.begin()+index);
        return COMP_OK;
    }

    return COMP_OUT_OF_BOUNDS;
}

void CapContainer::cap_clear() {

    // Clear complete list.
    this->imageCaptures.clear();
}

uint8_t CapContainer::cap_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageCaptures.size() && index2 < this->imageCaptures.size()) {

        shared_ptr<ImgCapture> ptr1 = this->imageCaptures[index1];
        shared_ptr<ImgCapture> ptr2 = this->imageCaptures[index2];
        ptr1.swap(ptr2);

        return COMP_OK;
    }
    return COMP_OUT_OF_BOUNDS;
}

uint8_t CapContainer::cap_firstIndexOf(uint8_t captureID) {

    // Get iterator for capture instances.
    auto delIt = this->imageCaptures.begin();

    // Iterate list and delete capture, if found.
    while (delIt != this->imageCaptures.end()) {

        // Found capture id.
        if ((*delIt)->getCapId() == captureID)
            return delIt - this->imageCaptures.begin();

        delIt++;
    }

    // Not found.
    return 0xFF;

}

shared_ptr<ImgCapture> CapContainer::cap_get(uint8_t index) {

    if (index < this->imageCaptures.size())
        return this->imageCaptures.at(index);

}

uint8_t CapContainer::cap_getSize() {

    return this->imageCaptures.size();
}

vector<shared_ptr<ImgCapture>>::iterator CapContainer::cap_getBeginElement() {
    return this->imageCaptures.begin();
}

vector<shared_ptr<ImgCapture>>::iterator CapContainer::cap_getEndElement() {
    return this->imageCaptures.end();
}
