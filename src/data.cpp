#include "tue/config/data.h"
#include "tue/config/map.h"

#include <boost/make_shared.hpp>

#include "tue/config/yaml_emitter.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

Data::Data()
{
    addNode(boost::make_shared<Map>(""), -1);
}

// ----------------------------------------------------------------------------------------------------

Data::~Data()
{
}

// ----------------------------------------------------------------------------------------------------

void data_merge(Data& c1, NodeIdx me_idx, const Data& c2, NodeIdx other_idx, std::string& error)
{
    NodePtr& n1 = c1.nodes[me_idx];
    const NodePtr& n2 = c2.nodes[other_idx];

    if (n1->type() != n2->type())
    {
        error += "Trying to merge two nodes of different types.\n";
        return;
    }

    if (n1->type() == ARRAY)
    {
        // n1's sequence is completely replaced by n2's sequence
        n1 = n2;
    } else
    {
        Map* map1 = static_cast<Map*>(n1.get());
        const Map* map2 = static_cast<const Map*>(n2.get());

        // Merge children
        const std::map<Label, NodeIdx>& children = map2->map_;
        for(std::map<Label, NodeIdx>::const_iterator it2 = children.begin(); it2 != children.end(); ++it2)
        {
            const Label& label2 = it2->first;
            std::map<Label, NodeIdx>::iterator it1 = map1->map_.find(label2);
            if (it1 == map1->map_.end())
            {
                // c1 does not have that child, so add it
                map1->map_[label2] = c1.addNode(c2.nodes[it2->second], me_idx);
            }
            else
            {
                // n1 already has that child. Merge them.
                data_merge(c1, it1->second, c2, it2->second, error);
            }
        }

        // Merge values
        const std::map<Label, Variant>& values = map2->values;
        for(std::map<Label, Variant>::const_iterator it2 = values.begin(); it2 != values.end(); ++it2)
        {
            map1->values[it2->first] = it2->second;
        }
    }
}

// ----------------------------------------------------------------------------------------------------

bool Data::add(const Data& other)
{
    std::string error;
    data_merge(*this, this->root(), other, other.root(), error);

    if (!error.empty())
    {
        std::cout << "Data::add: " << error << std::endl;
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const Data& d)
{
    YAMLEmitter emitter;
    emitter.emit(d, d.root(), out);
    return out;
}

}

} // end namespace tue

