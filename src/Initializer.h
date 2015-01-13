/*
 * Initializer.h
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#ifndef INITIALIZER_H_
#define INITIALIZER_H_

// Configuration file name
#define AMBER_OBU_CONF_FILE "obu.conf"

// Modules of the application
#include "Module.h"
#include "ModuleEvent.h"
#include "ModuleIO.h"
#include "ModuleImgProcessing.h"
#include "ModuleNetworking.h"

// Value instances for module setup
#include "Value.h"

// Event handling classes.
#include "evt-handling/EvtProcessor.h"

// Input/Output handling classes.
#include "io-handling/Config.h"
#include "io-handling/GpsAdafruit.h"
#include "io-handling/MPU6050.h"
#include "io-handling/OBDReader.h"

// Image handling classes.
#include "img-handling/ImgCapture.h"
#include "img-handling/OpPrepare.h"
#include "img-handling/OpCurveDetection.h"

// Network handling classes.
#include "nw-handling/NW_SocketInterface.h"

#include <memory>
#include <unistd.h>

// Return values.
typedef enum {
    INIT_OK,
    INIT_ERR_LOAD,
    INIT_ERR_DEV_UNKNOWN,
    INIT_ERR_DEV_OPEN,
    INIT_ERR_DEV_SETUP,
    INIT_ERR_DEV_APPEND
}initStatus;

class Initializer : public Module {
public:

    // Class Construction / Destruction.
    Initializer();
    virtual ~Initializer();

    uint8_t setup();    // Setup modules.
    void spawn();       // Spawn threads

protected:

    // Implementation of virtual parent methods.
    virtual uint8_t countMsgFromChildren();
    virtual uint8_t pollMsgFromChildren();
    virtual shared_ptr<Message_M2M> processMsg(shared_ptr<Message_M2M>);

private:

    uint8_t setUpIO(shared_ptr<Config> conf);       // Set up input/output module.
    uint8_t setUpImgProc(shared_ptr<Config> conf);  // Set up image processing module.
    uint8_t setUpEvtProc(shared_ptr<Config> conf);  // Set up event processing module.

    shared_ptr<Config> conf;    // Configuration file instance.

    ModuleEvent evt;    // Event processing module.
    ModuleIO io;        // Input/output processing module.
    ModuleImgProcessing proc;   // Image processing module.
    ModuleNetworking nw;    // Network processing module.

    static void printErr(uint8_t status, string location);  // Print setup error messages.
    static shared_ptr<NetworkCommunicator> createComm(
            string addr,
            string port,
            string iface,
            uint8_t commID,
            uint8_t devID); // Create network communication instance.

};

#endif /* INITIALIZER_H_ */
