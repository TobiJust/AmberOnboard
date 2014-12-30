/*
 * Initializer.cpp
 *
 *  Created on: 25.12.2014
 *      Author: Daniel Wagenknecht
 */

#include "Initializer.h"

Initializer::Initializer() { }

Initializer::~Initializer() { }

uint8_t Initializer::setup() {

    // Configuration file.
    shared_ptr<Config> conf(new Config(AMBER_OBU_CONF_FILE));

    // cerr << "Initializer: loading config" << endl;

    // Load configuration from file.
    uint8_t status = conf->load();
    if (status) {
        conf->save();
        return INIT_ERR_LOAD;
    }

    // cerr << "Initializer: setup io" << endl;

    // Set up io module.
    status = setUpIO(conf);
    if (status) {
        conf->save();
        return status;
    }

    // cerr << "Initializer: setup img processing" << endl;

    // Set up image processing module.
    status = setUpImgProc(conf);
    if (status) {
        conf->save();
        return status;
    }

    // cerr << "Initializer: setup networking" << endl;

    // Set up network module.
    status = setUpNetworking(conf);
    if (status)
        conf->save();

    // cerr << "Initializer: Done" << endl;

    this->io.addChild(conf);

    return status;
}

void Initializer::run() {

    cerr << "Initializer: running" << endl;

    // Create threads using the module's run methods.
    /*
     */
    thread thread_Img(&ModuleImgProcessing::run, &(this->proc));
    cerr << "Initializer: thread1 created" << endl;
    thread thread_IO(&ModuleIO::run, &(this->io));
    cerr << "Initializer: thread2 created" << endl;
    thread thread_NW(&ModuleNetworking::run, &(this->nw));
    cerr << "Initializer: thread3 created" << endl;

    cerr << "Initializer: waiting" << endl;

    // Wait until threads have terminated.
    /*
     */
        if(thread_Img.joinable())
            thread_Img.join();

    if(thread_IO.joinable())
        thread_IO.join();
    if(thread_NW.joinable())
        thread_NW.join();

    cerr << "Initializer: leaving" << endl;

}

uint8_t Initializer::setUpIO(shared_ptr<Config> conf) {

    // Create terminal instances.
    for (uint8_t index= 0; index < conf->getTermCount(); index++) {

        // Get terminal options.
        terminal term;
        conf->getTermAt(index, term);

        // Create and set up terminal.
        shared_ptr<Terminal> terminal(new Terminal(term.path, term.baud));
        if (!terminal->initialize()) return INIT_ERR_DEV_SETUP;

        // Add child to io module.
        this->io.addChild(dynamic_pointer_cast<Child>(terminal));
    }

    // TODO: implement io children.

    /*

    // Get accelerometer options.
    acc acc;
    conf->getAcc(acc);

    cerr << "Initializer: setup acc" << endl;

    // Create accelerometer child.
    if (acc.type == ACC_TYPE_MPU6050) {

        cerr << "Initializer: setup creating mpu" << endl;

        // Create and set up MPU6050.
        shared_ptr<MPU6050> mpu6050(new MPU6050(acc.addr, acc.path));

        cerr << "Initializer: initializing mpu" << endl;

        if (!mpu6050->initialize()) return INIT_ERR_DEV_SETUP;

        cerr << "Initializer: adding child" << endl;

        // Add child to io module.
        this->io.addChild(dynamic_pointer_cast<Child>(mpu6050));

        cerr << "Initializer: acc done" << endl;

    } else
        return INIT_ERR_DEV_UNKNOWN;

    // Create gps receiver instance.
    gps gps;
    conf->getGPS(gps);

    cerr << "Initializer: setup gps" << endl;

    // Create gps child.
    if (gps.type == GPS_TYPE_ADAFRUIT) {

        // Create and set up Adafruit receiver.
        shared_ptr<GpsAdafruit> adafruit(new GpsAdafruit(gps.path, gps.baud));
        if (!adafruit->initialize()) return INIT_ERR_DEV_SETUP;

        // Add child to io module.
        this->io.addChild(dynamic_pointer_cast<Child>(adafruit));

    } else
        return INIT_ERR_DEV_UNKNOWN;
     */

    return INIT_OK;
}

uint8_t Initializer::setUpImgProc(shared_ptr<Config> conf) {

    cerr << "Inner cap is " << (uint16_t)conf->getInnerCap() << endl;
    cerr << "Outer cap is " << (uint16_t)conf->getOuterCap() << endl;

    // Create objects for image capture.
    shared_ptr<CamCapture> outer(new CamCapture(conf->getOuterCap(), 1));
    shared_ptr<CamCapture> inner(new CamCapture(conf->getInnerCap(), 1));
    // shared_ptr<CamCapture> inner = outer;

    // Try opening cameras.
    if (!outer->openCapture()) return INIT_ERR_DEV_OPEN;
    if (!inner->openCapture()) return INIT_ERR_DEV_OPEN;

    // TODO: Include JPEG-quality option
    // Initialize arguments.
    shared_ptr<cv::Mat> prep_mat(new cv::Mat(480, 640, CV_8UC1));
    shared_ptr<ValMat> prep_ValMat(new ValMat(prep_mat));
    shared_ptr<ValInt> prep_ValScale(new ValInt(4));
    shared_ptr<ValInt> prep_ValPosX(new ValInt(5));
    shared_ptr<ValInt> prep_ValPosY(new ValInt(5));
    shared_ptr<ValInt> prep_ValQuali(new ValInt(75));

    // Create OpPrepare for image preparation and set arguments.
    shared_ptr<OpPrepare> prep_Op(new OpPrepare);
    prep_Op->setValue("Capture0", prep_ValMat);
    prep_Op->setValue("Capture1", prep_ValMat);
    prep_Op->setValue(ARG_SCALE, prep_ValScale);
    prep_Op->setValue(ARG_POS_X, prep_ValPosX);
    prep_Op->setValue(ARG_POS_Y, prep_ValPosY);
    prep_Op->setValue(ARG_JPEG_QUALITY, prep_ValQuali);

    // Create executor for image preparation.
    shared_ptr<ImgOpExecutor> prep_Exe(new ImgOpExecutor);

    // Append image capture instances in right order.
    if (conf->getOuterCap() == conf->getPrimaryCap()) {

        uint8_t status = EXEC_OK;

        // Append image capture instances.
        status  =prep_Exe->cap_append(dynamic_pointer_cast<ImgCapture>(outer));
        status +=prep_Exe->cap_append(dynamic_pointer_cast<ImgCapture>(inner));

        if (status) return INIT_ERR_DEV_APPEND;

    } else if (conf->getInnerCap() == conf->getPrimaryCap()) {

        uint8_t status = EXEC_OK;

        // Append image capture instances.
        status  =prep_Exe->cap_append(dynamic_pointer_cast<ImgCapture>(inner));
        status +=prep_Exe->cap_append(dynamic_pointer_cast<ImgCapture>(outer));

        if (status) return INIT_ERR_DEV_APPEND;

    } else {
        cerr << "Unknown device" << endl;
        return INIT_ERR_DEV_UNKNOWN;
    }

    // Append operator to corresponding executor.
    prep_Exe->op_append(dynamic_pointer_cast<ImgOperator>(prep_Op));

    // Finally add executors to image processing module.
    this->proc.addChild(prep_Exe);
    if (this->proc.exec_append(PREPARE, prep_Exe))
        return INIT_ERR_DEV_APPEND;

    return INIT_OK;
}

uint8_t Initializer::setUpNetworking(shared_ptr<Config> conf) {

    // Get server structs.
    server rt_Struct, nonrt_Struct;
    conf->getServRT(rt_Struct);
    conf->getServNonRT(nonrt_Struct);

    // Create and initialize real time connection.
    shared_ptr<NW_SocketInterface> rt_Iface(new NW_SocketInterface(
            AF_INET,
            SOCK_STREAM,
            rt_Struct.address,
            rt_Struct.port,
            rt_Struct.iface));

    uint8_t status = rt_Iface->initialize();

    cerr << "Initializer: network inited with " << (int)status  << endl;

    if (status) return INIT_ERR_DEV_SETUP;

    // TODO: Integrate device ID.

    shared_ptr<ProcDataFrame> rt_Frame(new ProcDataFrame);
    shared_ptr<ProcPayload> rt_Payload(new ProcPayload(123));

    shared_ptr<NetworkCommunicator> rt_Com(new NetworkCommunicator);
    if (!rt_Com->appenProc(rt_Payload)) return INIT_ERR_DEV_APPEND;
    if (!rt_Com->appenProc(rt_Frame)) return INIT_ERR_DEV_APPEND;
    if (!rt_Com->appenProc(rt_Iface)) return INIT_ERR_DEV_APPEND;

    if (!this->nw.com_append(rt_Com)) return INIT_ERR_DEV_APPEND;

    this->nw.addChild(rt_Com);
    rt_Com->attachObserver(&(this->nw));

    return INIT_OK;



    // Create and initialize real time connection.
    shared_ptr<NW_SocketInterface> nonrt_Iface(new NW_SocketInterface(
            AF_INET,
            SOCK_STREAM,
            nonrt_Struct.address,
            nonrt_Struct.port,
            nonrt_Struct.iface));
    if (nonrt_Iface->initialize()) return INIT_ERR_DEV_SETUP;

    // TODO: Integrate device ID.

    shared_ptr<ProcDataFrame> nonrt_Frame(new ProcDataFrame);
    shared_ptr<ProcPayload> nonrt_Payload(new ProcPayload(123));

    shared_ptr<NetworkCommunicator> nonrt_Com(new NetworkCommunicator);
    if (!nonrt_Com->appenProc(nonrt_Payload)) return INIT_ERR_DEV_APPEND;
    if (!nonrt_Com->appenProc(nonrt_Frame)) return INIT_ERR_DEV_APPEND;
    if (!nonrt_Com->appenProc(nonrt_Iface)) return INIT_ERR_DEV_APPEND;

    if (!this->nw.com_append(nonrt_Com)) return INIT_ERR_DEV_APPEND;

    this->nw.addChild(nonrt_Com);
    nonrt_Com->attachObserver(&(this->nw));

    return INIT_OK;
}
















