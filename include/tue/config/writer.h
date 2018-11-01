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

    bool writeGroup(const std::string& name);

    bool writeArray(const std::string& name);

    bool addArrayItem();

    bool end();

    template<typename T>
    void setValue(const std::string& name, const T& value)
    {
        Variant var(value);
        Label label = cfg_->getOrAddLabel(name);
        cfg_->nodes[idx_]->setValue(label, var);
    }

    bool endArray() { return end(); }
    bool endGroup() { return end(); }
    bool endArrayItem() { return end(); }

    inline DataConstPointer data() const { return DataConstPointer(cfg_, idx_); }

private:

    boost::shared_ptr<Data> cfg_;
    NodeIdx idx_;

};

} // end namespace tue

} // end namespace config

#endif
