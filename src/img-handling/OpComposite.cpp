/** \brief      Class implementing the composite pattern for image operators.
 *
 * \details     Implements composite pattern to combine different image operators.
 * \author      Daniel Wagenknecht
 * \version     2014-11-28
 * \class       OpComposite
 */

#include "OpComposite.h"

/** \brief Constructor.
 *
 *  Constructor of OpComposite instances, calling parent in initializer list with operator type 'type'.
 *
 *  \param type The operator type.
 */
OpComposite::OpComposite(uint8_t type) : ImgOperator(type, 1) {}

/** \brief Destructor.
 *
 *  Destructor of OpComposite instances.
 */
OpComposite::~OpComposite() { }

/** \brief Connect result and parameter.
 *
 *  Connects the result values identified by 'resultName' with the parameter identified by 'paramName'.
 *  This way, results of one operator in the composite implementation can be input for a
 *  successor one.
 *
 *  \param resultName Name of the result, which gets connected to a parameter.
 *  \param paramName Name of the parameter, which the result value gets connected to.
 */
void OpComposite::connect(string resultName, string paramName) {

    // Find resultName occurrence.
    auto connIt = this->connections.find(resultName);

    // Does not exist, create it
    if(connIt == this->connections.end())
        this->connections.insert(make_pair(resultName, paramName));

    // Exists, set new value.
    else
        connIt->second = paramName;

}

/**
 * Not needed directly.
 */
void OpComposite::createCaptures(uint8_t captureCount) { }

/** \brief Sets option.
 *
 *  Tries to find a composite leaf which the option 'val' as identifier 'name'
 *  could be applied to.
 *  Returns status indicator.
 *
 *  \param name Name of the option to set.
 *  \param val The actual option which gets set.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::setValue(string name, const shared_ptr<Value> &val) {

    uint8_t result=0;

    // Iterate operators.
    for (auto leafIt : this->imageOperators) {

        // Try setting value.
        uint8_t tmpResult = leafIt->setValue(name,val);

        // Value matched, return.
        if (tmpResult != ERR_NO_SUCH_KEY && tmpResult != OK)
            return tmpResult;
        else if (tmpResult == OK)
            result = tmpResult;
    }

    return result;
}

/** \brief Gets option.
 *
 *  Iterates all leafs to find the option specified by 'name' and writes it to 'val',
 *  if existing.
 *  Returns status indicator.
 *
 *  \param name Name of the option to get.
 *  \param val The target which gets set.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::getValue(string name, shared_ptr<Value> &val) {

    uint8_t result=0;

    for (auto leafIt : this->imageOperators) {

        // Try getting value.
        result = leafIt->getValue(name,val);

        // Value matched or error, return.
        if (result!=ERR_NO_SUCH_KEY)
            return result;
    }

    return ERR_NO_SUCH_KEY;
}

/** \brief Check if operator is initialized.
 *
 *  Iterates all leafs to check if they are initialized.
 *
 *  \return true, if all leafs are initialized, false otherwise.
 */
bool OpComposite::initialized() {

    for (auto leafIt : this->imageOperators) {

        // Value matched, return.
        if (!leafIt->initialized())
            return false;
    }

    return true;
}

/** \brief Get capture count.
 *
 *  Iterates all leafs and determines the highest number of captures needed.
 *
 *  \return The number of captures necessary for this composite operator.
 */
uint8_t OpComposite::getCaptureCount() {

    uint8_t result = 0;

    for (auto leafIt : this->imageOperators) {

        // Value matched, return.
        if (leafIt->getCaptureCount() > result)
            result = leafIt->getCaptureCount();
    }

    return result;
}

/** \brief Process operations.
 *
 *  Iterates over operators and processes their image operations and writes results into 'results'.
 *  Returns status indicator.
 *
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::process(unordered_map<string,shared_ptr<Value>> &results) {

    uint8_t status = 0;

    auto leafIt = this->imageOperators.begin();

    while (leafIt != this->imageOperators.end()) {

        // Temp variable.
        unordered_map<string,shared_ptr<Value>> tmp;

        // Apply operator.
        status = (*leafIt)->apply(tmp);

        // An error occurred.
        if (status != OK)
            return status;

        // There are more operators pending, so try setting their parameters.
        if ((leafIt+1) != this->imageOperators.end())

            // Iterate over results and set parameters of successor.
            for (auto connIt : this->connections) {

                auto resIt = tmp.find(connIt.first);
                shared_ptr<Value> dasdsadsa = (resIt->second);
                if(resIt != tmp.end())
                    (*(leafIt+1))->setValue(connIt.second, resIt->second);
            }

        // Insert results to list.
        results.insert(tmp.begin(), tmp.end());

        leafIt++;
    }

    return status;

}

/** \brief Append image operator.
 *
 *  Appends image operator 'op' as leaf to composite.
 *  Returns status indicator.
 *
 *  \param op Operator to append.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::op_append(shared_ptr<ImgOperator> &op) {

    // Last of the 256 possible indices is reserved.
    if(this->imageOperators.size() < 0xFF) {

        if (op) {
            this->imageOperators.push_back(op);
            return COMP_OK;
        }

        return COMP_INVALID_REFERENCE;
    }

    return COMP_OUT_OF_BOUNDS;
}

/** \brief Delete image operator.
 *
 *  Deletes image operator 'op' from composite.
 *  Returns status indicator.
 *
 *  \param op Operator to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::op_delete(shared_ptr<ImgOperator> &op) {

    // Pointer must not point to NULL.
    if (op) {

        // Get iterator for operator instances.
        auto delIt = this->imageOperators.begin();

        // Iterate list and delete operator, if found.
        while (delIt != this->imageOperators.end()) {

            if (*delIt == op) {
                this->imageOperators.erase(delIt);
                return COMP_OK;
            }

            delIt++;
        }
    }

    return COMP_INVALID_REFERENCE;
}

/** \brief Delete image operator.
 *
 *  Deletes image operator at index 'index' from composite.
 *  Returns status indicator.
 *
 *  \param index Index of the operator to delete.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::op_delete(uint8_t index) {

    // Index in list bounds.
    if ((uint32_t)index < this->imageOperators.size()) {

        // Erase from list.
        this->imageOperators.erase(this->imageOperators.begin()+index);
        return COMP_OK;
    }

    return COMP_OUT_OF_BOUNDS;
}

/** \brief Clear image operator list.
 *
 *  Deletes all image operators from composite.
 */
void OpComposite::op_clear() {

    // Clear complete list.
    this->imageOperators.clear();
}

/** \brief Swaps image operators indices.
 *
 *  Swaps image operator at index 'index1' with the one at 'index2'.
 *  Returns status indicator.
 *
 *  \param index1 Index of the first operator.
 *  \param index2 Index of the second operator.
 *  \return 0 in case of success, an error code otherwise.
 */
uint8_t OpComposite::op_swap(uint8_t index1, uint8_t index2) {

    // Indices in list bounds.
    if (index1 < this->imageOperators.size() && index2 < this->imageOperators.size()) {

        this->imageOperators[index1].swap(this->imageOperators[index2]);
        return COMP_OK;
    }
    return COMP_OUT_OF_BOUNDS;
}

/** \brief Find first operator of given type.
 *
 *  Finds the first operator of type 'opType' and returns its index, if existing.
 *
 *  \param opType Operator type to search for.
 *  \return Index of the operator.
 */
uint8_t OpComposite::op_firstIndexOf(uint8_t opType) {

    // Get iterator for operator instances.
    auto delIt = this->imageOperators.begin();

    // Iterate list and delete operator, if found.
    while (delIt != this->imageOperators.end()) {

        // Found capture id.
        if ((*delIt)->getType() == opType)
            return delIt - this->imageOperators.begin();

        delIt++;
    }

    // Not found.
    return 0xFF;
}
