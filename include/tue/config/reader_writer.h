#ifndef TUE_CONFIG_CONFIG_READER_WRITER_H_
#define TUE_CONFIG_CONFIG_READER_WRITER_H_

#include "tue/config/types.h"
#include "tue/config/data.h"

namespace tue
{

namespace config
{

class Configuration;

class ReaderWriter
{

public:

    ReaderWriter();

    virtual ~ReaderWriter();

    bool read(const std::string& name);

    bool end();

    bool next();

    template<typename T>
    bool value(const std::string& name, T& value) const
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

    void addError(const std::string& msg);

private:

    NodeIdx idx_;

    NodeIdx scope_;

    boost::shared_ptr<Configuration> cfg_;

    std::string error_;

};

} // end namespace config

} // end namespace tue

#endif
