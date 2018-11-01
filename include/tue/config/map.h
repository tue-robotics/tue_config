#ifndef TUE_CONFIG2_MAP_H_
#define TUE_CONFIG2_MAP_H_

#include "node.h"

namespace tue
{

namespace config
{

class Map : public Node
{

public:

    Map(const Label& name) : Node(name, MAP) {}

    bool readGroup(const Label& label, NodeIdx& idx) const
    {
        std::map<Label, NodeIdx>::const_iterator it = map_.find(label);
        if (it == map_.end())
            return false;

        idx = it->second;
        return true;
    }

    bool value(const Label& label, Variant& value) const
    {
        std::map<Label, Variant>::const_iterator it = values.find(label);
        if (it == values.end())
            return false;

        value = it->second;
        return true;
    }

    // addGroup never returns false, therefore overwriting is possible. Use readGroup before to prevent overwriting
    bool addGroup(const Label& label, const NodeIdx& n, NodeIdx& idx)
    {
        map_[label] = n;
        idx = n;
        return true;
    }

    bool setValue(const Label& label, const Variant& value)
    {
        values[label] = value;
        return true;
    }

    virtual bool empty() const
    {
        return (map_.empty() && values.empty());
    }

    // --- COPY

    NodePtr deepCopy(const Data& source, NodeIdx target_idx, Data& target) const;

    std::map<Label, NodeIdx> map_;

    std::map<Label, Variant> values;

};

}

} // end namespace tue

#endif
