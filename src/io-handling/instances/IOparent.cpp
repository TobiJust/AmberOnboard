/** \brief      Abstract parent for io handling.
 *
 * \details     Abstract base class for input / output handling.
 * \author      Daniel Wagenknecht
 * \version     2014-12-18
 * \class       IOparent
 */

#include "IOparent.h"

/** \brief Constructor.
 *
 *  Default constructor of IOparent instances.
 */
IOparent::IOparent() {
    this->active=false;
}

/** \brief Destructor.
 *
 *  Destructor of IOparent instances.
 */
IOparent::~IOparent() { }

/** \brief Sets activity status.
 *
 *  Sets the instances activity status to 'active'.
 *
 *  \param active The new activity indicator.
 */
void IOparent::setActive(bool active) {
    this->active=active;
}

/** \brief Check instance activity.
 *
 *  Returns the instances current activity status.
 *
 *  \return true if instance is active, false otherwise.
 */
bool IOparent::isActive() {
    return this->active;
}

/** \brief Safely reads from input.
 *
 *  Safely reads from input, which means a readopen is called and the
 *  origin port / file configuration is restored  after operation. Exemplary usage of this method
 *  is while dealing with the stdin/stdout of the current terminal at "/dev/tty".
 *  Not resetting the port before closing the application in this case causes strange behavior
 *  of the console, since echo and canonical input get manipulated.
 *  Returns status indicator.
 *
 *  \param input Target to write result to.
 *  \param delim Delimiter to stop the read process at.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::readSafe(string &input, uint8_t delim) {

    // Read input.
    uint8_t status = this->readOpen(input, delim);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

/** \brief Opens input and reads from it.
 *
 *  Opens the port / file before reading from it.
 *  Note: no reopen is forced when calling the actual open method.
 *  Returns status indicator.
 *
 *  \param input Target to write result to.
 *  \param delim Delimiter to stop the read process at.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::readOpen(string &input, uint8_t delim) {

    uint8_t status = IO_OK;

    // Open file if not already open:
    if (!this->isOpen())
        status = this->open(OPEN_R);

    // An error occurred.
    if (status)
        return status;

    status = this->read(input, delim);

    return status;

}

/** \brief Read from input.
 *
 *  Reads from the port / file until either the instance is deactivated or 'delim' was read.
 *  The result gets written to 'input'.
 *  Returns status indicator.
 *
 *  \param input Target to write result to.
 *  \param delim Delimiter to stop the read process at.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::read(string &input, uint8_t delim) {

    if (!this->isOpen())
        return IO_ERR_OPEN;

    // Resulting stringstream.
    stringstream stream("");

    // Last received byte.
    uint8_t current=0;

    this->rw_mutex.lock();

    // Read characters until delimiter is found.
    do {
        current = this->getChar();
        stream << current;
    } while (current != delim && this->active);

    this->rw_mutex.unlock();

    // Set resulting string.
    input=stream.str();

    return IO_OK;

}

/** \brief Safely writes string to output.
 *
 *  Safely writes to the port / file, which means the origin port / file configuration is restored after operation.
 *  Exemplary usage of this method is while dealing with the stdin/stdout of the current terminal at "/dev/tty".
 *  Not resetting the port before closing the application in this case causes strange behavior
 *  of the console, since echo and canonical input get manipulated.
 *  Returns status indicator.
 *
 *  \param output String which gets written.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::writeSafe(string output) {

    uint8_t status = write(output);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

/** \brief Writes string to output.
 *
 *  Forces a port / file reopen and appends 'output' to freshly opened file.
 *  Note: Opening a file in write mode deletes the content before writing to it.
 *  Returns status indicator.
 *
 *  \param output String which gets written.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::write(string output) {

    uint8_t status = IO_OK;

    // Open file anyway.
    status = this->open(OPEN_W, true);

    // An error occurred.
    if (status)
        return status;

    status = this->append(output);

    return status;

}

/** \brief Safely appends string to output.
 *
 *  Safely appends string to the port / file, which means the origin port / file configuration is restored after operation.
 *  Exemplary usage of this method is while dealing with the stdin/stdout of the current terminal at "/dev/tty".
 *  Not resetting the port before closing the application in this case causes strange behavior
 *  of the console, since echo and canonical input get manipulated.
 *  Returns status indicator.
 *
 *  \param output String which gets written.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::appendSafe(string output) {

    uint8_t status = this->appendOpen(output);

    // An error occurred.
    if (status)
        return status;

    // Reset file/port configuration.
    status = this->reset();

    return status;

}

/** \brief Opens output in append mode and writes to it.
 *
 *  Opens the port / file before writing to it.
 *  Note: no reopen is forced when calling the actual open method.
 *  Returns status indicator.
 *
 *  \param output String which gets written.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t IOparent::appendOpen(string output) {

    uint8_t status = IO_OK;

    // Open file if not already open:
    if (!this->isOpen())
        status = this->open(OPEN_A);

    // An error occurred.
    if (status)
        return status;

    status = this->append(output);

    return status;

}

/** \brief Opens output in append mode and writes to it.
 *
 *  Appends string 'output' to file / port.
 *  Returns status indicator.
 *
 *  \param output String which gets written.
 *  \return 0 in case of success, an error code otherwise.
 */

uint8_t IOparent::append(string output) {

    this->rw_mutex.lock();

    uint8_t status = this->putString(output);

    this->rw_mutex.unlock();

    return status;

}
