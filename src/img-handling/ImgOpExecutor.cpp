/** \brief      Class for executing image operations.
 *
 * \details     Class for executing operations on mat objects.
 * \author      Daniel Wagenknecht
 * \version     2014-11-18
 * \class       ImgOpExecutor
 */

#include "ImgOpExecutor.h"

ResultContainer::ResultContainer() {}
ResultContainer::~ResultContainer() {}

ImgOpExecutor::ImgOpExecutor() { }

ImgOpExecutor::ImgOpExecutor(shared_ptr<ImgCapture> &capture) {
    if (capture)
        this->imageCaptures.push_back(capture);
}

ImgOpExecutor::~ImgOpExecutor() { }

/** \brief Append image operator.
 *
 *  Appends image operator 'op' to execution list.
 *  Returns status indicator.
 *
 *  \param op Operator to append.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::op_append(shared_ptr<ImgOperator> op) {

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

/** \brief Delete image operator.
 *
 *  Deletes image operator 'op' from execution list.
 *  Returns status indicator.
 *
 *  \param op Operator to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::op_delete(shared_ptr<ImgOperator> op) {

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

/** \brief Delete image operator.
 *
 *  Deletes image operator at index 'index' from execution list.
 *  Returns status indicator.
 *
 *  \param index Index of the operator to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::op_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageOperators.size()) {

        // Erase from list.
        this->imageOperators.erase(this->imageOperators.begin()+index);
        return EXEC_OK;
    }

    return EXEC_OUT_OF_BOUNDS;
}

/** \brief Clear image operator list.
 *
 *  Deletes all image operators from execution list.
 */
void ImgOpExecutor::op_clear() {

    // Clear complete list.
    this->imageOperators.clear();
}

/** \brief Swaps image operators indices.
 *
 *  Swaps image operator at index 'index1' with the one at 'index2'.
 *  Returns status indicator.
 *
 *  \param index1 Index of the first operator.
 *  \param index2 Index of the second operator.
 *  \return 0 in case of success, an error code otherwise.
 */
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

/** \brief Find first operator of given type.
 *
 *  Finds the first operator of type 'opType' and returns its index, if existing.
 *
 *  \param opType Operator type to search for.
 *  \return Index of the operator.
 */
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

/** \brief Append image capture.
 *
 *  Appends image capture 'capture' to list of captures.
 *  Returns status indicator.
 *
 *  \param capture Capture to append.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::cap_append(shared_ptr<ImgCapture> capture) {

    this->captureMutex.lock();
    // Last of the 256 possible indices is reserved.
    if(this->imageCaptures.size() < 0xFF) {

        if (capture) {
            this->imageCaptures.push_back(capture);
            this->captureMutex.unlock();
            return EXEC_OK;
        }

        this->captureMutex.unlock();
        return EXEC_INVALID_REFERENCE;
    }

    this->captureMutex.unlock();
    return EXEC_OUT_OF_BOUNDS;
}

/** \brief Delete image capture.
 *
 *  Deletes image capture 'capture' from list of captures.
 *  Returns status indicator.
 *
 *  \param capture Capture to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::cap_delete(shared_ptr<ImgCapture> capture) {

    this->captureMutex.lock();

    // Pointer must not point to NULL.
    if (capture) {

        // Get iterator for capture instances.
        auto delIt = this->imageCaptures.begin();

        // Iterate list and delete capture, if found.
        while (delIt != this->imageCaptures.end()) {

            if (*delIt == capture) {
                this->imageCaptures.erase(delIt);
                this->captureMutex.unlock();
                return EXEC_OK;
            }

            delIt++;
        }
    }

    this->captureMutex.unlock();
    return EXEC_INVALID_REFERENCE;
}

/** \brief Delete image capture.
 *
 *  Deletes image capture at index 'index' from list of captures.
 *  Returns status indicator.
 *
 *  \param index Index of the capture to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::cap_delete(uint8_t index) {

    this->captureMutex.lock();

    // Index in list bounds.
    if ((uint32_t)index < this->imageCaptures.size()) {

        // Erase from list.
        this->imageCaptures.erase(this->imageCaptures.begin()+index);
        this->captureMutex.unlock();
        return EXEC_OK;
    }

    this->captureMutex.unlock();
    return EXEC_OUT_OF_BOUNDS;
}

/** \brief Clear image capture list.
 *
 *  Deletes all image captures from from list of captures.
 */
void ImgOpExecutor::cap_clear() {

    this->captureMutex.lock();

    // Clear complete list.
    this->imageCaptures.clear();

    this->captureMutex.unlock();

}

/** \brief Swaps image capture indices.
 *
 *  Swaps image capture at index 'index1' with the one at 'index2'.
 *  Returns status indicator.
 *
 *  \param index1 Index of the first capture.
 *  \param index2 Index of the second capture.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::cap_swap(uint8_t index1, uint8_t index2) {

    this->captureMutex.lock();

    // Indices in list bounds.
    if (index1 < this->imageCaptures.size() && index2 < this->imageCaptures.size()) {

        this->imageCaptures[index1].swap(this->imageCaptures[index2]);

        this->captureMutex.unlock();
        return EXEC_OK;
    }

    this->captureMutex.unlock();
    return EXEC_OUT_OF_BOUNDS;
}

/** \brief Find capture with given id.
 *
 *  Finds the first capture with id 'captureID'.
 *
 *  \param captureID Capture ID to search for.
 *  \return Index of the capture.
 */
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

/** \brief Sets result.
 *
 *  Sets the result value identified by 'identifier' to value 'target'.
 *  Creates the key-value pair, if not existing.
 *  Returns status indicator.
 *
 *  \param identifier The result identifier.
 *  \param target The result value.
 *  \return 0 in case of success, an error code otherwise.
 */
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

/** \brief Gets result.
 *
 *  Gets the result value identified by 'identifier' and writes it to value 'target'.
 *  Returns status indicator.
 *
 *  \param identifier The result identifier.
 *  \param target The value instance to write the result to.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::getResult(string identifier, shared_ptr<Value> &target) {

    this->producer.lock();

    // Try getting value.
    uint8_t status =  this->results.getValue(identifier, target);

    this->producer.unlock();

    return status;
}

/** \brief Sets value.
 *
 *  Sets the value identified by 'identifier' to value 'target'.
 *  Returns status indicator.
 *
 *  \param identifier The value identifier.
 *  \param target The new value.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::setValue(string identifier, shared_ptr<Value> target) {

    this->producer.lock();

    // Get iterator for operator instances.
    auto opIt = this->imageOperators.begin();

    // Iterate list of execution.
    while (opIt != this->imageOperators.end())

        (*opIt)->setValue(identifier, target);

    this->producer.unlock();

    return OK;
}

/** \brief Executes image operations.
 *
 *  Iterates list of image operators executes their operations..
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ImgOpExecutor::execute() {

    try {

        uint8_t resultCount = 0;

        // At least primary image source exists.
        if (this->imageCaptures.size()) {

            this->captureMutex.lock();

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
                        // temp = newest;  // TODO: Actually bind additional cameras.
                        temp = this->imageCaptures[capture]->getFrame();
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

                this->captureMutex.unlock();

            } else this->captureMutex.unlock();
        }

        return resultCount;

    } catch(const std::system_error& e) {
        std::cout << "\033[1;31m ImgOpExecutor: Caught system_error with code \033[0m " << e.code()
                                                                  << " meaning " << e.what() << '\n';
    }

    return 0;
}

/** \brief threads run method.
 *
 *  Run method of execution thread.
 */
int ImgOpExecutor::run() {

    while (!this->isTerminating()) {

        execute();
        notifyObservers();
    }

    return 0;

}
