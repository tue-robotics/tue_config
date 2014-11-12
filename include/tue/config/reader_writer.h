#ifndef TUE_CONFIG_CONFIG_READER_WRITER_H_
#define TUE_CONFIG_CONFIG_READER_WRITER_H_

#include "tue/config/types.h"
#include "tue/config/data.h"

// Sync
#include <ctime>

namespace tue
{

enum RequiredOrOoptional
{
    REQUIRED,
    OPTIONAL
};

namespace config
{

class Configuration;

class ReaderWriter
{

public:

    ReaderWriter();

    virtual ~ReaderWriter();

    // ---- READING -----------------------------------------------------------------------

    bool read(const std::string& name);

    bool end();

    bool next();

    template<typename T>
    bool value(const std::string& name, T& value, tue::RequiredOrOoptional opt = REQUIRED) const
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

    void print() const;

    ReaderWriter limitScope() const;

    bool add(const ReaderWriter& rw);

    bool readArray(const std::string& name, tue::RequiredOrOoptional opt = OPTIONAL) { return read(name); }
    bool readGroup(const std::string& name, tue::RequiredOrOoptional opt = OPTIONAL) { return read(name); }
    bool endArray() { return end(); }
    bool endGroup() { return end(); }
    bool nextArrayItem() { return next(); }


    // ---- WRITING -----------------------------------------------------------------------

    bool writeGroup(const std::string& name);

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


    // ---- ADDITIONAL -----------------------------------------------------------------------

    std::string toYAMLString() const;

    void addError(const std::string& msg);

    bool hasError() const { return !error_.empty(); }

    const std::string& error() const { return error_; }

    bool loadFromYAMLFile(const std::string& filename);

    bool sync();

    friend std::ostream& operator<< (std::ostream& out, const ReaderWriter& rw);

private:

    NodeIdx idx_;

    NodeIdx scope_;

    boost::shared_ptr<Configuration> cfg_;

    std::string error_;


    // Syncing

    std::string filename_;

    std::time_t source_last_write_time_;

};

} // end namespace config

} // end namespace tue

#endif
