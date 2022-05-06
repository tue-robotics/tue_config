#ifndef TUE_CONFIG_READER_WRITER_H_
#define TUE_CONFIG_READER_WRITER_H_

#include "tue/config/data.h"
#include "tue/config/resolve_config.h"
#include "tue/config/types.h"

#include <iostream>

// Sync
#include <ctime>

namespace tue
{

namespace config
{

struct Error
{
    std::string message;
};

class Data;

class ReaderWriter
{

public:

    ReaderWriter();

    ReaderWriter(DataPointer& cfg);

    virtual ~ReaderWriter();

    // ---- READING -----------------------------------------------------------------------

    template<typename T>
    /**
     * @brief value read value of child with key 'name'
     * @param name name of the key
     * @param value object to write the value to
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key not available or variable type isn't matching
     */
    bool value(const std::string& name, T& value, RequiredOrOptional opt = REQUIRED)
    {
        Label label;
        if (!cfg_->getLabel(name, label))
            return false;

        Variant v;
        if (!cfg_->nodes[idx_]->value(label, v))
        {
            if (opt == REQUIRED)
                addError("Expected property '" + name + "', not found.");
            return false;
        }

        if (!v.getValue(value))
        {
            if (opt == REQUIRED)
                addError("Property '" + name + "' has invalid type.");
            return false;
        }

        return true;
    }

    /**
     * @brief limitScope Limit the data to the current position. All parents and siblings incl. their child aren't
     * accesible anymore.
     * @return ReaderWriter with data limited to current position
     */
    ReaderWriter limitScope() const;

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
     * @brief source get the source file of the data in this object
     * @return string of the source file
     */
    const std::string& source() const { return cfg_->source_; }


    // ---- WRITING -----------------------------------------------------------------------

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
     * @brief setValue<string> set child value with key 'name' and value 'value',
     * value is stripped from leading and trailing whitespace
     * @param name name of the key
     * @param value value of the value
     */
    void setValue(const std::string& name, std::string value)
    {
        trim(value);

        Variant var(value);
        Label label = cfg_->getOrAddLabel(name);
        cfg_->nodes[idx_]->setValue(label, var);
    }

    /**
     * @brief addArrayItem create a new item in the array
     * @return indicates succes, can fail if current node isn't in an array
     */
    bool addArrayItem();

    /**
     * @brief endArrayItem go back to the array level
     * @return indicates succes, can fail if current node ins't an array item
     */
    bool endArrayItem();

    /**
     * @brief setSource set the source file of the data in this object
     * @param source string of the source file
     */
    void setSource(const std::string& source) { cfg_->source_ = source; }


    // ---- ADDITIONAL -----------------------------------------------------------------------

    /**
     * @brief toYAMLString convert data (from current reading/writing point) to yaml string
     * @return yaml string
     */
    std::string toYAMLString() const;

    void addError(const std::string& msg);

    bool hasError() const { return !error_->message.empty(); }

    const std::string& error() const { return error_->message; }

    /**
     * @brief loadFromSDFFile loads a sdf file into a ReaderWriter class
     * @param filename input file
     * @return indicates success
     */
    bool loadFromSDFFile(const std::string& filename);

    /**
     * @brief loadFromXMLFile loads a xml file into a ReaderWriter class
     * @param filename input file
     * @return indicates success
     */
    bool loadFromXMLFile(const std::string& filename);

    /**
     * @brief loadFromYAMLFile loads a yaml file into a ReaderWriter class
     * @param filename input file
     * @return indicates success
     */
    bool loadFromYAMLFile(const std::string& filename, const ResolveConfig& resolve_config = ResolveConfig::defaultConfig());

    /**
     * @brief sync re-read the source file if the file has changed since last reading time.
     * @return indicates succes, can fail if last changed time is incorrect or unsupported extension is used.
     */
    bool sync();

    /**
     * @brief operator << stream operator
     * @param out
     * @param rw ReaderWriter object
     * @return
     */
    friend std::ostream& operator<< (std::ostream& out, const ReaderWriter& rw);

    /**
     * @brief data get the data from the current reading/writing point
     * @return DataPointer
     */
    inline DataPointer data() const { return DataPointer(cfg_, idx_); }

    /**
     * @brief setErrorContext
     * @param context context string
     */
    inline void setErrorContext(const std::string& context) { error_context_.reset(new std::string(context)); }

    /**
     * @brief setShortErrorContext
     * @param context context string
     */
    inline void setShortErrorContext(const std::string& context)
    {
        cfg_->nodes[idx_]->setName(context);
    }

private:

    /**
     * @brief read read child with key 'name' of type ARRAY or MAP
     * @param name key of the child
     * @param type node type of the child, ARRAY or MAP
     * @param opt RequiredOrOptional
     * @return indicates succes, can fail if key isn't avalaible or child is of incorrect type
     */
    bool read(const std::string& name, const NodeType type, RequiredOrOptional opt = OPTIONAL);

    /**
     * @brief end go to parent node.
     * @return indicates succes, can fail if current node is the scope or if node doesn't have a parent
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

    NodeIdx scope_; // Index of highest node of the scope

    boost::shared_ptr<Data> cfg_; // Data pointer

    boost::shared_ptr<Error> error_; // Error pointer

    boost::shared_ptr<std::string> error_context_; // Error context pointer


    // Syncing

    std::string filename_; // filename of the source file

    std::time_t source_last_write_time_; // last writing time of the source file at time of reading.

    ResolveConfig resolve_config_; // Needs to be stored for sync

};

} // end namespace config

} // end namespace tue

#endif
