#ifndef TUE_CONFIG2_WRITER_H_
#define TUE_CONFIG2_WRITER_H_

#include "tue/config/configuration.h"

namespace tue
{

namespace config
{

class Writer
{

public:

    Writer(Configuration& cfg, NodeIdx idx = 0);

    virtual ~Writer();

    bool writeGroup(const std::string& name);

    bool writeArray(const std::string& name);

    bool nextArrayItem();

    bool end();

    template<typename T>
    void setValue(const std::string& name, const T& value)
    {
        Variant var(value);
        Label label = cfg_->getOrAddLabel(name);
        cfg_->nodes[idx_]->setValue(label, var);
    }

private:

    Configuration* cfg_;
    NodeIdx idx_;

};

} // end namespace tue

} // end namespace config

#endif
