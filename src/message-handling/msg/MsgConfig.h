/*
 * MsgSetConfig.h
 *
 *  Created on: 09.11.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef MSGSETCONFIG_H_
#define MSGSETCONFIG_H_

#include "Msg.h"
#include "../../Opt.h"

#include <queue>

typedef enum {
    CONF_IO,
    CONF_IMG_PROC
}confGroup;


// Abstract MsgSetConfig parent.
class MsgConfig : public Msg {
public:
    MsgConfig(int msgType, int confGroup);
    virtual ~MsgConfig()=0;

    void addOption(Opt* option);
    Opt* getNextOption();
    int getOptionCount();
    int getConfGroup();
protected:
    int confGroup;
    queue<Opt*> options;
};


// Concrete MsgSetConfig child for IO configuration.
class MsgIOConfig : public MsgConfig {
public:
    MsgIOConfig();
    virtual ~MsgIOConfig();
    MsgIOConfig* clone();
};


// Concrete MsgSetConfig child for image processing.
class MsgImgProcConfig : public MsgConfig {
public:
    MsgImgProcConfig();
    virtual ~MsgImgProcConfig();
    MsgImgProcConfig* clone();
};

#endif /* MSGSETCONFIG_H_ */
