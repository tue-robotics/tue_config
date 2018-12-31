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

    bool readArray(const std::string& name, const RequiredOrOptional opt = OPTIONAL) { return read(name, ARRAY, opt); }
    bool readGroup(const std::string& name, const RequiredOrOptional opt = OPTIONAL) { return read(name, MAP, opt); }
    bool endArray() { return end(); }
    bool endGroup() { return end(); }
    bool nextArrayItem() { return next(); }

    bool hasArray(const std::string& name) { return hasChild(name, ARRAY); }
    bool hasGroup(const std::string& name) { return hasChild(name, MAP); }

    inline DataConstPointer data() const { return DataConstPointer(cfg_, idx_); }

private:

    bool read(const std::string& name, const NodeType type, const RequiredOrOptional opt = OPTIONAL);

    bool end();

    bool next();

    bool hasChild(const std::string& name, NodeType type) const;

    NodeIdx idx_;

    boost::shared_ptr<const Data> cfg_;

};

} // end namespace config

} // end namespace tue

#endif
