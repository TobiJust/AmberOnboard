/*
 * ImgOpExecutor.cpp
 *
 *  Created on: 18.11.2014
 *      Author: Daniel Wagenknecht
 */

#include "ImgOpExecutor.h"

#include <unistd.h>

ResultContainer::ResultContainer() {}
ResultContainer::~ResultContainer() {}

ImgOpExecutor::ImgOpExecutor() { }

ImgOpExecutor::ImgOpExecutor(shared_ptr<ImgCapture> &capture) {

    if (capture)
        this->imageCaptures.push_back(capture);
}

ImgOpExecutor::~ImgOpExecutor() {

}

uint8_t ImgOpExecutor::op_append(shared_ptr<ImgOperator> &op) {

    // Last of the 256 possible indices is reserved.
    if(this->imageOperators.size() < 0xFF) {

        if (op) {
            this->imageOperators.push_back(op);
            return EXEC_OK;
        }

        return EXEC_INVALID_REFERENCE;
    }

    return EXEC_OUT_OF_BOUNDS;
}

uint8_t ImgOpExecutor::op_delete(shared_ptr<ImgOperator> &op) {

    // Pointer must not point to NULL.
    if (op) {

        // Get iterator for operator instances.
        auto delIt = this->imageOperators.begin();

        // Iterate list and delete operator, if found.
        while (delIt != this->imageOperators.end()) {

            if (*delIt == op) {
                this->imageOperators.erase(delIt);
                return EXEC_OK;
            }

            delIt++;
        }
    }

    return EXEC_INVALID_REFERENCE;
}

uint8_t ImgOpExecutor::op_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageOperators.size()) {

        // Erase from list.
        this->imageOperators.erase(this->imageOperators.begin()+index);
        return EXEC_OK;
    }

    return EXEC_OUT_OF_BOUNDS;
}

void ImgOpExecutor::op_clear() {

    // Clear complete list.
    this->imageOperators.clear();
}

uint8_t ImgOpExecutor::op_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageOperators.size() && index2 < this->imageOperators.size()) {

        shared_ptr<ImgOperator> ptr1 = this->imageOperators[index1];
        shared_ptr<ImgOperator> ptr2 = this->imageOperators[index2];
        ptr1.swap(ptr2);

        return EXEC_OK;
    }
    return EXEC_OUT_OF_BOUNDS;
}

uint8_t ImgOpExecutor::op_firstIndexOf(uint8_t opType) {

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

uint8_t ImgOpExecutor::cap_append(shared_ptr<ImgCapture> &capture) {

    // Last of the 256 possible indices is reserved.
    if(this->imageCaptures.size() < 0xFF) {

        if (capture) {
            this->imageCaptures.push_back(capture);
            return EXEC_OK;
        }

        return EXEC_INVALID_REFERENCE;
    }

    return EXEC_OUT_OF_BOUNDS;
}

uint8_t ImgOpExecutor::cap_delete(shared_ptr<ImgCapture> &capture) {

    // Pointer must not point to NULL.
    if (capture) {

        // Get iterator for capture instances.
        auto delIt = this->imageCaptures.begin();

        // Iterate list and delete capture, if found.
        while (delIt != this->imageCaptures.end()) {

            if (*delIt == capture) {
                this->imageCaptures.erase(delIt);
                return EXEC_OK;
            }

            delIt++;
        }
    }

    return EXEC_INVALID_REFERENCE;
}

uint8_t ImgOpExecutor::cap_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageCaptures.size()) {

        // Erase from list.
        this->imageCaptures.erase(this->imageCaptures.begin()+index);
        return EXEC_OK;
    }

    return EXEC_OUT_OF_BOUNDS;
}

void ImgOpExecutor::cap_clear() {

    // Clear complete list.
    this->imageCaptures.clear();
}

uint8_t ImgOpExecutor::cap_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageCaptures.size() && index2 < this->imageCaptures.size()) {

        shared_ptr<ImgCapture> ptr1 = this->imageCaptures[index1];
        shared_ptr<ImgCapture> ptr2 = this->imageCaptures[index2];
        ptr1.swap(ptr2);

        return EXEC_OK;
    }
    return EXEC_OUT_OF_BOUNDS;
}

uint8_t ImgOpExecutor::cap_firstIndexOf(uint8_t captureID) {

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

uint8_t ImgOpExecutor::setResult(string identifier, shared_ptr<Value> &target) {

    this->producer.lock();

    // Try setting value.
    uint8_t status = this->results.setValue(identifier, target);

    // Not a valid key yet, so create it.
    if (status == ERR_NO_SUCH_KEY) {
        this->results.createValue(identifier, target);
        this->producer.unlock();
        return OK;
    }
    // Wrong type, so recreate key-value pair.
    if (status == ERR_TYPE_MISMATCH) {
        this->results.deleteKey(identifier);
        this->results.createValue(identifier, target);
        this->producer.unlock();
        return OK;
    }

    this->producer.unlock();

    return status;

}

uint8_t ImgOpExecutor::getResult(string identifier, shared_ptr<Value> &target) {

    this->producer.lock();

    // Try getting value.
    uint8_t status =  this->results.getValue(identifier, target);

    this->producer.unlock();

    return status;
}

uint8_t ImgOpExecutor::execute() {

    uint8_t resultCount = 0;

    // At least primary image source exists.
    if (this->imageCaptures.size()) {

        // Next capture from camera + temp pointer for additional ones.
        shared_ptr<cv::Mat> newest(this->imageCaptures[0]->getFrame());
        shared_ptr<cv::Mat> temp;

        // 'newest' does not point to NULL.
        if (newest) {

            // Get iterator for operator instances.
            auto opIt = this->imageOperators.begin();

            // Iterate list of execution.
            while (opIt != this->imageOperators.end()) {

                stringstream cap;
                cap << ARG_CAPTURE << (int)0;
                (*opIt)->setValue(cap.str(), shared_ptr<ValMat>(new ValMat(newest)));

                // If operator requires more than one capture.
                for (uint8_t capture=1;
                        capture < (*opIt)->getCaptureCount() && capture < this->imageCaptures.size();
                        capture++) {

                    cap.str(std::string());
                    cap.clear();
                    cap << ARG_CAPTURE << (int)capture;

                    // Get frame from image capture and set corresponding value of operator.
                    temp = newest;  // TODO: Actually bind additional cameras.
                    // temp = this->imageCaptures[capture]->getFrame();
                    shared_ptr<ValMat> argValue(new ValMat(temp));
                    (*opIt)->setValue(cap.str(), argValue);
                }

                // Process image from primary capture.
                unordered_map<string,shared_ptr<Value>> instanceResults;
                (*opIt)->apply(instanceResults);

                // Get iterator for results.
                auto resIt = instanceResults.begin();

                // Iterate list of results and add them to own list.
                while (resIt != instanceResults.end()) {

                    this->setResult(resIt->first, resIt->second);
                    resultCount++;
                    resIt++;
                }
                opIt++;
            }

        }
    }

    return resultCount;
}




int ImgOpExecutor::run() {

    while (true) {

        execute();
        notifyObservers();

    }

    return 0;

}
