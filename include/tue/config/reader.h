#ifndef TUE_CONFIG2_CONFIG_READER_H_
#define TUE_CONFIG2_CONFIG_READER_H_

#include "tue/config/types.h"
#include "tue/config/configuration.h"


namespace tue
{

namespace config
{

class Configuration;

class Reader
{

public:

    Reader(const tue::config::Configuration& cfg);

    virtual ~Reader();

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

private:

    NodeIdx idx_;

    const Configuration* cfg_;

};

} // end namespace config

} // end namespace tue

#endif
