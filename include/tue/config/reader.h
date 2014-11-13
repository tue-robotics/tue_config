#ifndef TUE_CONFIG2_CONFIG_READER_H_
#define TUE_CONFIG2_CONFIG_READER_H_

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

    Reader(const Data& cfg);

    Reader(const DataConstPtr& cfg);

    virtual ~Reader();

    bool read(const std::string& name);

    bool end();

    bool next();

    template<typename T>
    bool value(const std::string& name, T& value, RequiredOrOoptional opt = REQUIRED) const
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

    bool readArray(const std::string& name, RequiredOrOoptional opt = OPTIONAL) { return read(name); }
    bool readGroup(const std::string& name, RequiredOrOoptional opt = OPTIONAL) { return read(name); }
    bool endArray() { return end(); }
    bool endGroup() { return end(); }
    bool nextArrayItem() { return next(); }

private:

    NodeIdx idx_;

    DataConstPtr cfg_;

};

} // end namespace config

} // end namespace tue

#endif
