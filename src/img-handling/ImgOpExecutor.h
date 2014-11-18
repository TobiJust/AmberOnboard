/*
 * ImgOpExecutor.h
 *
 *  Created on: 18.11.2014
 *      Author: administrator
 */

#ifndef IMGOPEXECUTOR_H_
#define IMGOPEXECUTOR_H_

#include "ImgCapture.h"
#include "ImgOperator.h"
#include "ImgOpContainer.h"
#include "../Child.h"

#include <opencv2/highgui/highgui.hpp>

#include <mutex>
#include <vector>

using namespace std;


class ImgOpExecutor : public ImgOpContainer, public Child {
public:
    ImgOpExecutor(ImgCapture* capture);
    virtual ~ImgOpExecutor();

    void setImgCapture(ImgCapture* capture);
    ImgCapture* getImgCapture();

    bool addOp(ImgOperator* op);
    void clear();

    int run();

    int getResults(unordered_map<int,Value*>* results);
protected:
    int execute();
    void addPair(int key, Value* value);

    unordered_map<int,Value*> currentResults;
    ImgCapture* capInstance;
    vector<ImgOperator*> operators;
    mutex producer;

};

#endif /* IMGOPEXECUTOR_H_ */
