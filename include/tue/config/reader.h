#ifndef TUE_CONFIG_READER_H_
#define TUE_CONFIG_READER_H_

#include "tue/config/types.h"
#include "tue/config/data.h"


namespace tue
{

namespace config
{

class Data;

class Reader
{

public:

    Reader(const DataConstPointer& ptr);

    virtual ~Reader();

    template<typename T>
    /**
     * @brief value read value of child with key 'name'
     * @param name name of the key
     * @param value object to write the value to
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key not available or variable type isn't matching
     */
    bool value(const std::string& name, T& value, const RequiredOrOptional opt = REQUIRED) const
    {
        Label label;
        if (!cfg_->getLabel(name, label))
            return false;

        Variant v;
        if (!cfg_->nodes[idx_]->value(label, v))
            return false;

        if (!v.getValue(value))
            return false;

        return true;
    }

    /**
     * @brief readArray read the child with key 'name', which should be an array
     * @param name key of the array
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key isn't available or child is a group instead of an array
     */
    bool readArray(const std::string& name, const RequiredOrOptional opt = OPTIONAL) { return read(name, ARRAY, opt); }

    /**
     * @brief readGroup read the child with key 'name', which should be a group
     * @param name key of the group
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key isn't avalaible or child is an array instead of a group
     */
    bool readGroup(const std::string& name, const RequiredOrOptional opt = OPTIONAL) { return read(name, MAP, opt); }

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
     * @brief nextArrayItem go to the next item in the array, wrapping next() for readbility
     * @return indicates succes, can fail if current node isn't in an array or if it is the last item in the array
     */
    bool nextArrayItem() { return next(); }

    /**
     * @brief hasArray check if current node has a child, which is an array, with the key 'name'
     * @param name key of the possible child array
     * @return indicates succes, can fail if key isn't avalaible or child is an array instead of a group
     */
    bool hasArray(const std::string& name) { return hasChild(name, ARRAY); }

    /**
     * @brief hasGroup check if current node has a child, which is a group, with the key 'name'
     * @param name key of the possible child group
     * @return indicates succes, can fail if key isn't avalaible or child is a group instead of an array
     */
    bool hasGroup(const std::string& name) { return hasChild(name, MAP); }

    /**
     * @brief data get the data from the current reading/writing point
     * @return DataConstPointer
     */
    inline DataConstPointer data() const { return DataConstPointer(cfg_, idx_); }

private:

    /**
     * @brief read read child with key 'name' of type ARRAY or MAP
     * @param name key of the child
     * @param type node type of the child, ARRAY or MAP
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key isn't avalaible or child is of incorrect type
     */
    bool read(const std::string& name, const NodeType type, const RequiredOrOptional opt = OPTIONAL);

    /**
     * @brief end go to parent node.
     * @return indicates succes, can fail if current node doesn't have a parent
     */
    bool end();

    /**
     * @brief next go to next item in an array
     * @return indicates succes, can fail if current node isn't in an array or if it is the last item in the array
     */
    bool next();

    /**
     * @brief hasChild check if node has a child with key 'name' and type ARRAY or MAP
     * @param name key of the child
     * @param type node type of the child, ARRAY or MAP
     * @return indicates succes, can fail if key isn't avalaible or child is a group instead of an array
     */
    bool hasChild(const std::string& name, NodeType type) const;

    NodeIdx idx_; // Current node index

    boost::shared_ptr<const Data> cfg_; // Data pointer

};

} // end namespace config

} // end namespace tue

#endif
