/** \brief      Container class for storing values.
 *
 * \details     This class stores a variable number of different data types, encapsulated as Value objects.
 * \author      Daniel Wagenknecht
 * \version     2014-11-14
 * \class       ValContainer
 */

#include "ValContainer.h"

/** \brief Constructor.
 *
 *  Default Constructor of value container instances.
 */
ValContainer::ValContainer() { }

/** \brief Destructor.
 *
 *  Destructor of value container instances.
 */
ValContainer::~ValContainer() { }

/** \brief Creates new value for this container.
 *
 *  Creates a new Value of the type specified in 'value', accessible through key 'name'.
 *
 *  \param name Name to identify the value.
 *  \param value Instance to use as initial value.
 */
void ValContainer::createValue(string name, shared_ptr<Value> value) {
    config.insert(make_pair(name, value));
}

/** \brief Deletes a value from container.
 *
 *  Deletes the value with key 'name' from container.
 *
 *  \param name Name to identify the value.
 */
void ValContainer::deleteKey(string name) {
    config.erase(name);
}

/** \brief Sets a value in the container
 *
 *  Sets the value specified by 'name' to the Value instance specified by 'val'.
 *  Returns a status indicator for this operation.
 *
 *  \param name Name to identify the value.
 *  \param value New value instance.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ValContainer::setValue(string name, const shared_ptr<Value> &val) {

    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Type of argument does not match option type.
    if(val->getType() != valIt->second->getType())
        return ERR_TYPE_MISMATCH;

    // Switch Value type for setting appropriate data structure.
    switch (val->getType()) {
    case VAL_DOUBLE:
    {
        // Create and initialize child pointer.
        shared_ptr<ValDouble> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValDouble>(val);
        oldValue = dynamic_pointer_cast<ValDouble>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_MAT:
    {
        // Create and initialize child pointer.
        shared_ptr<ValMat> newValue, oldValue;

        newValue = dynamic_pointer_cast<ValMat>(val);
        oldValue = dynamic_pointer_cast<ValMat>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_INT:
    {
        // Create and initialize child pointer.
        shared_ptr<ValInt> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValInt>(val);
        oldValue = dynamic_pointer_cast<ValInt>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_STRING:
    {

        // Create and initialize child pointer.
        shared_ptr<ValString> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValString>(val);
        oldValue = dynamic_pointer_cast<ValString>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    case VAL_UCHAR_VECTOR:
    {
        // Create and initialize child pointer.
        shared_ptr<ValVectorUChar> newValue, oldValue;
        newValue = dynamic_pointer_cast<ValVectorUChar>(val);
        oldValue = dynamic_pointer_cast<ValVectorUChar>(valIt->second);

        // Set new value.
        oldValue->setValue(newValue->getValue());

        break;
    }

    default:
        return ERR_UNKNOWN;
    }

    return OK;

}

/** \brief Gets a value from container.
 *
 *  Writes the Value instance specified by 'name' to 'val'.
 *  Returns a status indicator.
 *
 *  \param name Name to identify the value.
 *  \param val Instance which after this operation holds the value.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t ValContainer::getValue(string name, shared_ptr<Value> &val) {

    // Get pair iterator for module name.
    auto valIt = config.find(name);

    // Key not found in map.
    if(valIt == config.end())
        return ERR_NO_SUCH_KEY;

    // Value is not initialized
    if(!valIt->second->isInitialized())
        return ERR_UNSET_VALUE;

    // Set Value.
    val=valIt->second;

    return OK;

}


/** \brief Checks if all values are initialized.
 *
 *  Iterates through all values and checks if they are initialized.
 *  Returns the result of that check.
 *
 *  \return true if all values are initialized, false otherwise.
 */
bool ValContainer::initialized() {

    // Check if all config values are initialized.
    for(auto it : config)
        if(!it.second->isInitialized())
            return false;

    return true;

}


/** \brief Gets value count.
 *
 *  Returns the number of values currently stored in this container.
 *
 *  \return number of values in this container.
 */
uint8_t ValContainer::getValueCount() {
    return config.size();

}
