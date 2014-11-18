/*
 * ImgOpExecutor.cpp
 *
 *  Created on: 18.11.2014
 *      Author: administrator
 */

#include "ImgOpExecutor.h"

ImgOpExecutor::ImgOpExecutor(ImgCapture* capture) {
    this->capInstance=capture;
}

ImgOpExecutor::~ImgOpExecutor() {

}

void ImgOpExecutor::setImgCapture(ImgCapture* capture) {
    this->capInstance=capture;
}

ImgCapture* ImgOpExecutor::getImgCapture() {
    return this->capInstance;
}

bool ImgOpExecutor::addOp(ImgOperator* op) {

    // Operator does not exist or is uninitialized.
    if (op == NULL || !op->initialized())
        return false;

    // Append operator to list.
    this->operators.push_back(op);

    // Add 'this' reference to ops list of containers.
    op->addContainer(this);

    return true;

}

void ImgOpExecutor::clear() {

    // While there are elements in the list.
    while (this->operators.size()) {

        // Erase 'this' from ops container list.
        ImgOperator* op = *(this->operators.begin());
        op->remContainer(this);

        // Erase operator from list.
        this->operators.erase(this->operators.begin());

        if (!op->getContainerCount())
            delete op;

    }

}

int ImgOpExecutor::getResults(unordered_map<int,Value*>* results) {

    // Initi return value.
    int returnValue = this->currentResults.size();

    // Add all new results to map.
    results->insert(this->currentResults.begin(), this->currentResults.end());

    // Clean up result member.
    this->currentResults.clear();

    // Signal executor thread, that data are polled.
    producer.unlock();

    return returnValue;

}

int ImgOpExecutor::execute() {

    int resultCount =0;

    // Next capture from camera.
    const cv::Mat* next = this->capInstance->getFrame();

    // Iterate over operators.
    for (auto it : this->operators) {

        // Process last captured image.
        unordered_map<int,Value*> instanceResults;
        it->process(next, &instanceResults);

        for (auto resIt : instanceResults) {

            resultCount++;
            addPair(resIt.first, resIt.second);
        }
    }

    // Free image memory.
    delete next;

    return resultCount;

}

void ImgOpExecutor::addPair(int key, Value* value) {

    // Check if key is already in list.
    auto keyIt = this->currentResults.find(key);

    if(keyIt != this->currentResults.end()) {

        // Get pointer from list.
        Value* tmp = this->currentResults.at(key);

        // Set new pointer...
        keyIt->second = value;

        // ... and delete old one.
        delete tmp;

    } else
        this->currentResults.insert(make_pair(key, value));

}

int ImgOpExecutor::run() {

    while (this->capInstance->isActive()) {

        producer.lock();
        producer.unlock();

        execute();
        notifyObservers();

        producer.lock();

    }

    return 0;

}
