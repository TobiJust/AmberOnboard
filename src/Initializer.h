/*
 * Initializer.h
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef INITIALIZER_H_
#define INITIALIZER_H_

#define AMBER_OBU_CONF_FILE "obu.conf"

#include "ModuleIO.h"
#include "ModuleImgProcessing.h"
#include "ModuleNetworking.h"
#include "Value.h"

#include "io-handling/Config.h"
#include "io-handling/GpsAdafruit.h"
#include "io-handling/MPU6050.h"
#include "io-handling/Terminal.h"

#include "img-handling/ImgCapture.h"
#include "img-handling/OpPrepare.h"

#include "nw-handling/NW_SocketInterface.h"

#include <memory>

typedef enum {
    INIT_OK,
    INIT_ERR_LOAD,
    INIT_ERR_DEV_UNKNOWN,
    INIT_ERR_DEV_OPEN,
    INIT_ERR_DEV_SETUP,
    INIT_ERR_DEV_APPEND
}initStatus;

class Initializer {
public:

    Initializer();
    virtual ~Initializer();

    uint8_t setup();
    void run();

    uint8_t setUpIO(shared_ptr<Config> conf);
    uint8_t setUpImgProc(shared_ptr<Config> conf);
    uint8_t setUpNetworking(shared_ptr<Config> conf);

private:

    ModuleIO io;
    ModuleImgProcessing proc;
    ModuleNetworking nw;

};

#endif /* INITIALIZER_H_ */
