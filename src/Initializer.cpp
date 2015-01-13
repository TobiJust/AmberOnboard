/** \brief      Initialization class of the application.
 *
 * \details     Main controlling class of the project, doing the initialization and spawning necessary module threads.
 *              Here, the configuration file is loaded and its options are distributed to all modules, as needed.
 * \author      Daniel Wagenknecht
 * \version     2014-12-25
 * \class       Initializer
 */

#include "Initializer.h"

/** \brief Constructor.
 *
 *  Default Constructor of Initializer instances.
 *  Registers the initializer on message hub for messages to respawning threads and terminating the application.
 */
Initializer::Initializer() {

    // Register Initializer for messages.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_RESPAWN);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_BROADCAST);
}

/** \brief Destructor.
 *
 *  Destructor of Initializer instances.
 */
Initializer::~Initializer() { }

/** \brief Set up application modules.
 *
 *  Loads configuration file and distributes its options to all application modules.
 *
 * \return Enum indicating the setup state.
 */
uint8_t Initializer::setup() {

    // Create configuration file object.
    this->conf = shared_ptr<Config>(new Config(AMBER_OBU_CONF_FILE));

    // Load configuration from file.
    uint8_t status = conf->load();

    // One or more errors occurred.
    // Print them and return setup status indicator.
    if (status) {

        vector<string> errors = conf->getErrors();
        for (auto err : errors)
            cerr << err << endl;
        return INIT_ERR_LOAD;
    }

    // Set up io module.
    status = setUpIO(conf);
    if (status)
        return status;

    // Set up image processing module.
    status = setUpImgProc(conf);
    if (status)
        return status;

    // Set up event module.
    status = setUpEvtProc(conf);
    if (status)
        return status;

    // Create Messages for spawning network threads.
    // These messages are polled from message hub as soon as the main thread of this instance is running.
    shared_ptr<M2M_Respawn> respawn(new M2M_Respawn);
    shared_ptr<ValInt> respID(new ValInt);
    respID->setValue(NW_REALTIME);
    respawn->setValue(ARG_RESPAWN_CHILD, respID);

    // Push message to hub.
    MsgHub::getInstance()->appendMsg(respawn);

    return status;
}

/** \brief Spawn all module threads.
 *
 *  Spawn the main modules threads and wait for them to terminate
 *  (which means the application itself should terminate).
 */
void Initializer::spawn() {

    // Create threads using the module's run methods.
    thread thread_Init(&Initializer::run, this);
    thread thread_Img(&ModuleImgProcessing::run, &(this->proc));
    thread thread_IO(&ModuleIO::run, &(this->io));
    thread thread_NW(&ModuleNetworking::run, &(this->nw));

    // Insert short delay to let the modules start before raising any events.
    sleep(1);

    // Start last thread for event generation.
    thread thread_Evt(&ModuleEvent::run, &(this->evt));

    // Wait until threads have terminated by joining them.
    if(thread_Init.joinable())
        thread_Init.join();
    if(thread_Img.joinable())
        thread_Img.join();
    if(thread_IO.joinable())
        thread_IO.join();
    if(thread_Evt.joinable())
        thread_Evt.join();
    if(thread_NW.joinable())
        thread_NW.join();
}

/** \brief Set up input/output handling.
 *
 *  Sets up the module for input/output handling, based on the options in the configuration file.
 *
 * \param conf Configuration file instance.
 * \return Enum indicating the setup state.
 */
uint8_t Initializer::setUpIO(shared_ptr<Config> conf) {

    // Child for managing sensor instances.
    shared_ptr<SensorIO> sensors(new SensorIO);

    // Get accelerometer options.
    i2cDev acc;
    uint8_t accType;
    conf->getAcc(acc);
    conf->getAccType(accType);

    cerr << "Initializer: setup acc" << endl;

    // Create accelerometer child.
    if (accType == T_MPU6050) {

        cerr << "Initializer: creating mpu" << endl;

        // Create and set up MPU6050.
        shared_ptr<MPU6050> mpu6050(new MPU6050(acc.addr, acc.path));

        cerr << "Initializer: initializing mpu" << endl;

        if (mpu6050->initialize()) {
            printErr(INIT_ERR_DEV_SETUP, "imu (mpu6050)");
            return INIT_ERR_DEV_SETUP;
        }

        cerr << "Initializer: adding child" << endl;

        // Add child to sensors.
        sensors->append(mpu6050);

        cerr << "Initializer: acc done" << endl;

    } else return INIT_ERR_DEV_UNKNOWN;

    // Create gps receiver instance.
    uartDev gps;
    uint8_t gpsType;
    conf->getGPS(gps);
    conf->getGPSType(gpsType);

    cerr << "Initializer: setup gps" << endl;

    // Create gps child.
    if (gpsType == T_ADAFRUIT) {

        // Create and set up Adafruit receiver.
        shared_ptr<GpsAdafruit> adafruit(new GpsAdafruit(gps.path, gps.baud));
        if (adafruit->initialize()) {
            printErr(INIT_ERR_DEV_SETUP, "gps (adafruit)");
            return INIT_ERR_DEV_SETUP;
        }

        cerr << "Init:  GPS adding" << endl;

        // Add child to sensors.
        sensors->append(adafruit);

    } else return INIT_ERR_DEV_UNKNOWN;

    // Create obd receiver instance.
    uartDev obd;
    conf->getOBD(obd);

    cerr << "Initializer: obd path is " << obd.path << endl;

    // Create and set up obd receiver.
    shared_ptr<OBDReader> elm(new OBDReader(obd.path, obd.baud));
    if (elm->reset()) {
        printErr(INIT_ERR_DEV_SETUP, "obd module");
        return INIT_ERR_DEV_SETUP;
    } else if (elm->setup()) {
        printErr(INIT_ERR_DEV_SETUP, "obd module");
        return INIT_ERR_DEV_SETUP;
    }

    // Add child to sensors.
    sensors->append(elm);

    this->io.addChild(sensors);
    this->io.setSensorIO(sensors);

    return INIT_OK;
}

/** \brief Set up image processing.
 *
 *  Sets up the module for image processing, based on the options in the configuration file.
 *
 * \param conf Configuration file instance.
 * \return Enum indicating the setup state.
 */
uint8_t Initializer::setUpImgProc(shared_ptr<Config> conf) {

    // TODO: delete capture id.
    // Video capture instances.
    capture out, in;

    // Get capture configuration.
    if (!conf->getOuterCap(out) || !conf->getInnerCap(in)) {
        printErr(INIT_ERR_LOAD, "capture");
        return INIT_ERR_LOAD;
    }

    // Create objects for image capture.
    shared_ptr<CamCapture> outer(new CamCapture(out.index, 1, out.fps));
    shared_ptr<CamCapture> inner(new CamCapture(in.index, 1, in.fps));

    // Try opening outer camera.
    if (!outer->openCapture()) {
        printErr(INIT_ERR_DEV_OPEN, "outer camera");
        return INIT_ERR_DEV_OPEN;
    }

    // Try opening inner camera.
    if (!inner->openCapture()) {
        printErr(INIT_ERR_DEV_OPEN, "inner camera");
        return INIT_ERR_DEV_OPEN;
    }

    // Initialization of image stream preparator.
    // Get jpeg compression.
    uint8_t compression=100;
    if (!conf->getJpegCompression(compression)) {
        printErr(INIT_ERR_LOAD, "compression");
        return INIT_ERR_LOAD;
    }

    // Initialize arguments.
    shared_ptr<cv::Mat> prep_mat(new cv::Mat(480, 640, CV_8UC1));
    shared_ptr<ValMat> prep_ValMat(new ValMat(prep_mat));
    shared_ptr<ValInt> prep_ValScale(new ValInt(4));
    shared_ptr<ValInt> prep_ValPosX(new ValInt(5));
    shared_ptr<ValInt> prep_ValPosY(new ValInt(5));
    shared_ptr<ValInt> prep_ValQuali(new ValInt(compression));

    // Create OpPrepare for image preparation and set arguments.
    shared_ptr<OpPrepare> prep_Op(new OpPrepare);
    prep_Op->setValue("Capture0", prep_ValMat);
    prep_Op->setValue("Capture1", prep_ValMat);
    prep_Op->setValue(ARG_SCALE, prep_ValScale);
    prep_Op->setValue(ARG_POS_X, prep_ValPosX);
    prep_Op->setValue(ARG_POS_Y, prep_ValPosY);
    prep_Op->setValue(ARG_JPEG_QUALITY, prep_ValQuali);

    // Initialization of curve detection.
    // Initialize arguments.
    shared_ptr<cv::Mat> curve_mat(new cv::Mat(480, 640, CV_8UC1));
    shared_ptr<ValMat> curve_ValMat(new ValMat(prep_mat));
    shared_ptr<ValInt> curve_ValAngle(new ValInt(47));
    shared_ptr<ValInt> curve_ValRadAtMeter(new ValInt(10));
    shared_ptr<ValInt> curve_ValAreaPix(new ValInt(80));
    shared_ptr<ValInt> curve_ValPix2Meter(new ValInt(160));
    shared_ptr<ValInt> curve_ValMultiplier(new ValInt(5));
    shared_ptr<ValInt> curve_ValMarkWidth(new ValInt(80));
    shared_ptr<ValInt> curve_ValThreshold(new ValInt(10));
    shared_ptr<ValInt> curve_ValMaxDist(new ValInt(10));
    shared_ptr<ValDouble> curve_ValOffset(new ValDouble(1.9));
    shared_ptr<ValDouble> curve_ValCamHeight(new ValDouble(3.0));
    shared_ptr<ValInt> curve_Active(new ValInt(1));

    // Create OpCurveDetection for curve detection and set arguments.
    shared_ptr<OpCurveDetection> curve_Op(new OpCurveDetection);
    curve_Op->setValue("Capture0", curve_ValMat);
    curve_Op->setValue(ARG_CAM_VIEW_ANGLE_V, curve_ValAngle);
    curve_Op->setValue(ARG_CAM_CALC_RADIUS_AT_METER, curve_ValRadAtMeter);
    curve_Op->setValue(ARG_CAM_MARKING_SEARCH_AREA_PIX, curve_ValAreaPix);
    curve_Op->setValue(ARG_CAM_PIX_TO_METER_K, curve_ValPix2Meter);
    curve_Op->setValue(ARG_CAM_WIDTH_MULTIPLIER, curve_ValMultiplier);
    curve_Op->setValue(ARG_CAM_MARK_WIDTH, curve_ValMarkWidth);
    curve_Op->setValue(ARG_CAM_THRESHOLD, curve_ValThreshold);
    curve_Op->setValue(ARG_CAM_MAX_DIST, curve_ValMaxDist);
    curve_Op->setValue(ARG_CAM_OFFSET, curve_ValOffset);
    curve_Op->setValue(ARG_CAM_HEIGHT, curve_ValCamHeight);
    curve_Op->setValue(ARG_OP_ACTIVE, curve_Active);

    // Initialize curve detection.
    if (curve_Op->initialize()) {
        printErr(INIT_ERR_DEV_SETUP, "Curve Img-Operator");
        return INIT_ERR_DEV_SETUP;
    }

    // Create executor for image preparation.
    shared_ptr<ImgOpExecutor> exe(new ImgOpExecutor);

    // Get primary camera index.
    uint8_t primary=0;
    if (!conf->getPrimeCap(primary)) {
        printErr(INIT_ERR_LOAD, "primary capture");
        return INIT_ERR_LOAD;
    }

    uint8_t status = EXEC_OK;

    // Append image capture instances in proper order.
    if (out.index == primary) {

        // Append image capture instances.
        status  =exe->cap_append(dynamic_pointer_cast<ImgCapture>(outer));
        status +=exe->cap_append(dynamic_pointer_cast<ImgCapture>(inner));

        // An error occurred.
        if (status) {
            printErr(INIT_ERR_DEV_APPEND, "capture");
            return INIT_ERR_DEV_APPEND;
        }

    } else if (in.index == primary) {

        // Append image capture instances.
        status  =exe->cap_append(dynamic_pointer_cast<ImgCapture>(inner));
        status +=exe->cap_append(dynamic_pointer_cast<ImgCapture>(outer));

        // An error occurred.
        if (status) {
            printErr(INIT_ERR_DEV_APPEND, "capture");
            return INIT_ERR_DEV_APPEND;
        }

        // An error occurred.
    } else {
        printErr(INIT_ERR_DEV_UNKNOWN, "capture");
        return INIT_ERR_DEV_UNKNOWN;
    }

    // TODO: delete this.
    status = exe->execute();
    if (status) {
        printErr(INIT_ERR_DEV_UNKNOWN, "image op executer");
        return INIT_ERR_DEV_UNKNOWN;
    }

    // Append operator to corresponding executor.
    exe->op_append(dynamic_pointer_cast<ImgOperator>(prep_Op));
    // exe->op_append(dynamic_pointer_cast<ImgOperator>(curve_Op));

    // Finally add executors to image processing module.
    this->proc.addChild(exe);
    if (this->proc.exec_append(PREPARE, exe)) {
        printErr(INIT_ERR_DEV_APPEND, "img operator");
        return INIT_ERR_DEV_APPEND;
    }

    return INIT_OK;
}

/** \brief Set up event processing.
 *
 *  Sets up the module for event processing, based on the options in the configuration file.
 *
 * \param conf Configuration file instance.
 * \return Enum indicating the setup state.
 */
uint8_t Initializer::setUpEvtProc(shared_ptr<Config> conf) {

    // Create event detector instance.
    shared_ptr<EvtProcessor> processor(new EvtProcessor);

    // Add operator for acceleration event detection.
    shared_ptr<OpAcceleration> accEvents(new OpAcceleration(14000));
    if (!processor->op_append(accEvents)) {
        printErr(INIT_ERR_DEV_APPEND, "event operator");
        return INIT_ERR_DEV_APPEND;
    }

    // Add operator for gyroscope event detection.
    shared_ptr<OpGyroscope> gyroEvents(new OpGyroscope(1500));
    if (!processor->op_append(gyroEvents)) {
        printErr(INIT_ERR_DEV_APPEND, "event operator");
        return INIT_ERR_DEV_APPEND;
    }

    // Finally add processors to event processing module.
    this->evt.evt_append(processor);
    this->evt.addChild(processor);

    return INIT_OK;
}

/** \brief Print out error messages.
 *
 *  Prints out the error message for a given error code and location of the programm.
 *
 * \param status Error code.
 * \param location The affected programm part.
 */
void Initializer::printErr(uint8_t status, string location) {

    switch (status) {
    case INIT_ERR_LOAD:
        cerr << "Unable to load '" << location << "'." << endl;
        break;
    case INIT_ERR_DEV_UNKNOWN:
        cerr << "Unknown device '" << location << "'." << endl;
        break;
    case INIT_ERR_DEV_OPEN:
        cerr << "Unable to open device '" << location << "'." << endl;
        break;
    case INIT_ERR_DEV_SETUP:
        cerr << "Unable to set up device '" << location << "'." << endl;
        break;
    case INIT_ERR_DEV_APPEND:
        cerr << "Unable to append '" << location << "'." << endl;
        break;
    default:
        break;
    }
}

/** \brief Count messages from module children.
 *
 *  Counts incoming messages from children.
 *  Since Initializer has no children, this function returns 0.
 *
 *  \return Number of child messages.
 */
uint8_t Initializer::countMsgFromChildren() {
    return 0;
}

/** \brief Polls messages from module children.
 *
 *  Polls incoming messages from children and counts the polled messages.
 *  This method implements child-module communication.
 *  Since Initializer has no children, this function does nothing and returns 0.
 *
 *  \return Number of polled child messages.
 */
uint8_t Initializer::pollMsgFromChildren() {
    return 0;
}

/** \brief Processes incoming messages from message hub.
 *
 *  Process oldest message for this module from message hub.
 *  This method implements inter-module communication.
 *  Returns the answer on the incoming message or a null pointer
 *  if no answer is needed.
 *
 *  \param msg Incoming message from other modules.
 *  \return Answer on incoming message or NULL.
 */
shared_ptr<Message_M2M> Initializer::processMsg(shared_ptr<Message_M2M> msg) {

    // Resulting message.
    shared_ptr<Message_M2M> result;

    // Process message depending on message type.
    switch (msg->getType()) {
    case MSG_RESPAWN:
    {
        // Get type of child thread to respawn.
        shared_ptr<Value> typeVal;
        msg->getValue(ARG_RESPAWN_CHILD, typeVal);

        // Do requested respawn.
        switch (dynamic_pointer_cast<ValInt>(typeVal)->getValue()) {

        // Respawn realtime communicator.
        case NW_REALTIME: {

            // Get configuration of realtime connection.
            server realtime;
            if (!conf->getRealTime(realtime)) {
                printErr(INIT_ERR_LOAD, "server params");
                return msg;
            }

            // Get device id.
            uint8_t devID=0;
            conf->getDeviceID(devID);

            // Create network communicator instance.
            shared_ptr<NetworkCommunicator> comm = createComm(realtime.target, realtime.port, realtime.iface, NW_TYPE_REALTIME, devID);

            // If comm does not point to null, communicator creation was successful.
            if (comm) {

                // Append network communicator to network module.
                if (this->nw.com_append(comm)){
                    printErr(INIT_ERR_DEV_APPEND, "communicator");
                    return msg;
                }

                // Add child to list of threaded children and run it.
                this->nw.addChild(comm);
                comm->attachObserver(&(this->nw));
                this->nw.runChild(this->nw.getChildCount()-1);

            } else return msg;
            break;
        }

        // Respawn deferred communicator.
        case NW_DEFERRED:
            break;
        default:
            break;
        }
        break;
    }
    case MSG_TERM_BROADCAST:
    {
        // Terminate is requested.
        this->terminate();
        break;
    }
    default:
        break;
    }

    return result;
}

/** \brief Create network communicator instance.
 *
 *  Creates and sets up a network communicator instance.
 *  The instance connects over network interface 'iface' to server at 'addr' on port 'port'.
 *
 *  \param addr Server address (IP or domain name).
 *  \param port Server port to connect to.
 *  \param iface Network interface to use for connection.
 *  \param commID Identifier of this network communicator.
 *  \param devID Id of this obu.
 *  \return Shared pointer to freshly created network communicator instance..
 */
shared_ptr<NetworkCommunicator> Initializer::createComm(string addr, string port, string iface, uint8_t commID, uint8_t devID) {

    shared_ptr<NetworkCommunicator> result;

    // Create and initialize communicator.
    shared_ptr<NW_SocketInterface> interface(new NW_SocketInterface(
            AF_UNSPEC,
            SOCK_STREAM,
            addr,
            port,
            iface));

    uint16_t status = 0;

    // Initialize server connection.
    if ((status=interface->initialize())) {
        printErr(INIT_ERR_DEV_SETUP, "server");
        return result;
    }

    // Processor instances for building up data frames.
    shared_ptr<ProcDataFrame> frame(new ProcDataFrame);
    shared_ptr<ProcPayload> payload(new ProcPayload(devID));

    // Network communicator instance.
    result = shared_ptr<NetworkCommunicator>(new NetworkCommunicator(commID));

    // Append Frame processors
    if (!result->appenProc(payload) ||
            !result->appenProc(frame) ||
            !result->appenProc(interface)) {
        printErr(INIT_ERR_DEV_APPEND, "frame processor");
        return shared_ptr<NetworkCommunicator>();
    }

    return result;
}
