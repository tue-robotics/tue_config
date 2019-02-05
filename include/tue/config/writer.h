#ifndef TUE_CONFIG_WRITER_H_
#define TUE_CONFIG_WRITER_H_

#include "tue/config/data.h"

namespace tue
{

namespace config
{

class Writer
{

public:

    Writer();

    Writer(DataPointer& cfg);

    virtual ~Writer();

    /**
     * @brief writeGroup starts writing a group. Or start extending it, if it already exists.
     * @param name of the group
     * @return indicates succes
     */
    bool writeGroup(const std::string& name);

    /**
     * @brief writeArray starts writing an array. Or start extending it, if it already exists.
     * @param name of the array
     * @return indicates succes
     */
    bool writeArray(const std::string& name);

    /**
     * @brief addArrayItem create a new item in the array
     * @return indicates succes, can fail if current node isn't in an array
     */
    bool addArrayItem();

    template<typename T>
    /**
     * @brief setValue set child value with key 'name' and value 'value'
     * @param name name of the key
     * @param value value of the value
     */
    void setValue(const std::string& name, const T& value)
    {
        Variant var(value);
        Label label = cfg_->getOrAddLabel(name);
        cfg_->nodes[idx_]->setValue(label, var);
    }

    /**
     * @brief endArray go to parrent of current array, wrapping end() for readibility
     * @return indicates succes, see end() for more information
     */
    bool endArray() { return end(); }

    /**
     * @brief endGroup go to the parrent of current group, wrapping end() for readbility
     * @return indicates succes, see end() for more information
     */
    bool endGroup() { return end(); }

    /**
     * @brief endArrayItem go back to the array level
     * @return indicates succes, can fail if current node ins't an array item
     */
    bool endArrayItem() { return end(); }

    /**
     * @brief data get the data from the current reading/writing point
     * @return DataConstPointer
     */
    inline DataConstPointer data() const { return DataConstPointer(cfg_, idx_); }

private:

    /**
     * @brief end go to parent node.
     * @return indicates succes, can fail if current node doesn't have a parent
     */
    bool end();

    NodeIdx idx_; // Current node index

    boost::shared_ptr<Data> cfg_;  // Data pointer

};

} // end namespace tue

} // end namespace config

#endif
