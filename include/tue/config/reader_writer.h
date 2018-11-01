#ifndef TUE_CONFIG_CONFIG_READER_WRITER_H_
#define TUE_CONFIG_CONFIG_READER_WRITER_H_

#include "tue/config/types.h"
#include "tue/config/data.h"

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

    bool read(const std::string& name, const NodeType type, RequiredOrOptional opt = OPTIONAL);

    bool end();

    bool next();

    template<typename T>
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

    void print() const;

    ReaderWriter limitScope() const;

//    bool add(const ReaderWriter& rw);

    bool readArray(const std::string& name, RequiredOrOptional opt = OPTIONAL) { std::cout << "readArray" << std::endl; return read(name, ARRAY, opt); }
    bool readGroup(const std::string& name, RequiredOrOptional opt = OPTIONAL) { std::cout << "readGroup" << std::endl; return read(name, MAP, opt); }
    bool endArray() { return end(); }
    bool endGroup() { return end(); }
    bool nextArrayItem() { return next(); }

    const std::string& source() const { return cfg_->source_; }


    // ---- WRITING -----------------------------------------------------------------------

    /**
     * @brief writeGroup
     * @param name
     * @return
     */
    bool writeGroup(const std::string& name);

    /**
     * @brief writeArray
     * @param name
     * @return
     */
    bool writeArray(const std::string& name);

    template<typename T>
    void setValue(const std::string& name, const T& value)
    {
        Variant var(value);
        Label label = cfg_->getOrAddLabel(name);
        cfg_->nodes[idx_]->setValue(label, var);
    }

    bool addArrayItem();

    bool endArrayItem();

    void setSource(const std::string& source) { cfg_->source_ = source; }


    // ---- ADDITIONAL -----------------------------------------------------------------------

    std::string toYAMLString() const;

    void addError(const std::string& msg);

    bool hasError() const { return !error_->message.empty(); }

    const std::string& error() const { return error_->message; }

    /**
     * @brief loadFromXMLFile loads a xml file into a ReaderWriter class
     * @param filename input file
     * @return indicates success
     */
    bool loadFromXMLFile(const std::string& filename);

    /**
     * @brief YAML loads a xml file into a ReaderWriter class
     * @param filename input file
     * @return indicates success
     */
    bool loadFromYAMLFile(const std::string& filename);

    bool sync();

    friend std::ostream& operator<< (std::ostream& out, const ReaderWriter& rw);

    inline DataPointer data() const { return DataPointer(cfg_, idx_); }

    inline void setErrorContext(const std::string& context) { error_context_.reset(new std::string(context)); }

    inline void setShortErrorContext(const std::string& context)
    {
        cfg_->nodes[idx_]->setName(context);
    }

private:

    NodeIdx idx_;

    NodeIdx scope_;

    boost::shared_ptr<Data> cfg_;

    boost::shared_ptr<Error> error_;

    boost::shared_ptr<std::string> error_context_;


    // Syncing

    std::string filename_;

    std::time_t source_last_write_time_;

};

} // end namespace config

} // end namespace tue

#endif
