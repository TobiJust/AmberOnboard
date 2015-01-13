/** \brief      Application module for image processing.
 *
 * \details     This class manages image processors.
 * \author      Daniel Wagenknecht
 * \version     2014-11-08
 * \class       ModuleImgProcessing
 */

#include "ModuleImgProcessing.h"

/** \brief Constructor.
 *
 *  Default Constructor of ModuleImgProcessing instances.
 *  Registers for messages regarding termination, commands and data acquisition.
 */
ModuleImgProcessing::ModuleImgProcessing() {

    // Register for messages.
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_DATA_ACQUIRED);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_EVENT_ACQUIRE);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_TERM_BROADCAST);
    MsgHub::getInstance()->attachObserverToMsg(this, MSG_COMMAND);

}

/** \brief Destructor.
 *
 *  Destructor of ModuleImgProcessing instances.
 */
ModuleImgProcessing::~ModuleImgProcessing() { }

/** \brief Appends image op executor processor to list.
 *
 *  Appends ImgOpExecutor specified by 'executor' to list of image op executors.
 *  Returns status indicator of the operation.
 *
 * \param executor ImgOpExecutor to add.
 * \return 0 in case of success, an error number otherwise.
 */
uint8_t ModuleImgProcessing::exec_append(string key, shared_ptr<ImgOpExecutor> executor) {

    // Executor reference must be valid and key not empty.
    if (!executor) return IMG_PROC_ERR_INVALID_REFERENCE;
    if (!key.length()) return IMG_PROC_ERR_UNKNOWN;

    // Add executor to list.
    this->executors.insert(make_pair(key, executor));
    return IMG_PROC_OK;
}

/** \brief Deletes an element from list of image op executors.
 *
 *  Deletes the element specified by 'key' from list of executors.
 *
 *  \param key The executors key.
 * \return 0 in case of success, an error number otherwise.
 */
uint8_t ModuleImgProcessing::exec_delete(string key) {

    // Key must not be empty.
    if (key.length()) return IMG_PROC_ERR_UNKNOWN;

    // Erase executor.
    this->executors.erase(key);
    return IMG_PROC_OK;
}

/** \brief Clears list of image op executors.
 *
 *  Completely clears list of image op executor instances.
 */
void ModuleImgProcessing::exec_clear() {

    this->executors.clear();
}

/** \brief Counts messages of children.
 *
 *  Counts the pending messages of all managed ImgOpExecutor instances
 *  and returns the result.
 *
 * \return Number of child messages.
 */
uint8_t ModuleImgProcessing::countMsgFromChildren() {

    // Children do not return any messages.
    return 0;
}

/** \brief Polls messages from module children.
 *
 *  Polls incoming messages from children and counts the polled messages.
 *  This method implements child-module communication.
 *
 *  \return Number of polled child messages.
 */
uint8_t ModuleImgProcessing::pollMsgFromChildren() {

    // Children do not return any messages..
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
shared_ptr<Message_M2M> ModuleImgProcessing::processMsg(shared_ptr<Message_M2M> msg) {

    // Resulting message.
    shared_ptr<Message_M2M> result;

    // Switch incoming message type.
    switch (msg->getType()) {
    case MSG_DATA_ACQUIRED:
    {
        // Create new dataset message instance
        shared_ptr<M2M_DataSet> set(new M2M_DataSet);

        // Find executor 'PREPARE', which generates the streamed images.
        shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;
        shared_ptr<vector<unsigned char>> tmp;

        // Preparator exists for this module (is not null)
        if (prep_Exe) {

            // Resulting image.
            shared_ptr<Value> imgResult;

            // Try getting stream image.
            // Set to empty in case of errors.
            if (prep_Exe->getResult(RES_ENCODED_JPEG, imgResult))
                set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar(tmp)));

            // There is an image available, so set the message field.
            else {
                shared_ptr<ValVectorUChar> jpegImg = dynamic_pointer_cast<ValVectorUChar>(imgResult);
                set->setValue(ARG_IMG, jpegImg);
            }

            // Preparator not found, set empty image.
        } else set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar(tmp)));

        // Set result and leave switch case.
        result = set;
        break;
    }
    case MSG_EVENT_ACQUIRE:
    {
        // Create new dataset message instance
        shared_ptr<M2M_EventDataSet> set(new M2M_EventDataSet);

        // Find executor 'PREPARE', which generates the streamed images.
        shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;
        shared_ptr<vector<unsigned char>> tmp;

        // Preparator exists for this module (is not null)
        if (prep_Exe) {

            // Resulting image.
            shared_ptr<Value> imgResult;

            // Try getting stream image.
            // Set to empty in case of errors.
            if (prep_Exe->getResult(RES_ENCODED_JPEG, imgResult))
                set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar(tmp)));

            // There is an image available, so set the message field.
            else {
                shared_ptr<ValVectorUChar> jpegImg = dynamic_pointer_cast<ValVectorUChar>(imgResult);
                set->setValue(ARG_IMG, jpegImg);
            }

            // Preparator not found, set empty image.
        } else set->setValue(ARG_IMG, shared_ptr<ValVectorUChar>(new ValVectorUChar(tmp)));

        // Set result and leave switch case.
        result = set;
        break;
    }
    case MSG_COMMAND:
    {
        // Get command type.
        shared_ptr<ValInt> cmdType(new ValInt);
        shared_ptr<Value> tmp = dynamic_pointer_cast<Value>(cmdType);
        string typeString = ARG_COMMAND_TYPE;
        msg->getValue(typeString, tmp);

        switch (cmdType->getValue()) {
        case CMD_SWAP_CAM: {

            // Swap primary and secondary cam of this executor.
            shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;
            prep_Exe->cap_swap(0,1);
            break;
        }
        case CMD_CURVE_ACTIVE: {

            // Set all image operators to active, which react on the value.
            shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;
            shared_ptr<ValInt> active_Val(new ValInt(1));
            string active = ARG_OP_ACTIVE;
            prep_Exe->setValue(active, active_Val);
            break;
        }
        case CMD_CURVE_INACTIVE: {

            // Set all image operators to inactive, which react on the value.
            shared_ptr<ImgOpExecutor> prep_Exe = this->executors.find(PREPARE)->second;
            shared_ptr<ValInt> active_Val(new ValInt(0));
            string active = ARG_OP_ACTIVE;
            prep_Exe->setValue(active, active_Val);
            break;
        }
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
