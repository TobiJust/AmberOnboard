/** \brief      Abstract base class for deviecs.
 *
 * \details     Abstract base class for device instances.
 * \author      Daniel Wagenknecht
 * \version     2015-01-04
 * \class       Device
 */

#include "Device.h"

/** \brief Constructor.
 *
 *  Default constructor of Device instances.
 */
Device::Device() {
    this->terminating=false;
}

/** \brief Destructor.
 *
 *  Destructor of Device instances.
 */
Device::~Device() { }

/** \brief Add value to results.
 *
 *  Add 'value' with key 'key' to 'values'.
 */
void Device::addValue(unordered_map<string, string> &values, string key, string value) {

    // Iterator to the key-th position in the map.
    auto it = values.find(key);

    // Change value, if key exists.
    if (it == values.end())
        values.insert(make_pair(key, value));

    // Create key-value pair otherwise.
    else
        it->second = value;
}

/** \brief Sets termination flag.
 *
 *  Sets termination flag.
 */
void Device::terminate() {
    this->terminating=true;
}

/** \brief Get termination value.
 *
 *  Returns whether device is terminating or not
 *
 *  \return true if terminate was called, false otherwise.
 */
bool Device::isTerminating() {
    return this->terminating;
}
