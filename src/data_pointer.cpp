#include "tue/config/data_pointer.h"
#include "tue/config/data.h"
#include "tue/config/map.h"
#include "tue/config/sequence.h"
#include "tue/config/yaml_emitter.h"

#include <boost/make_shared.hpp>

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

NodeIdx deepCopy(const Data& source, NodeIdx source_idx, NodeIdx parent_idx, Data& target, NodeIdx target_idx = -1)
{    
    const NodePtr& n = source.nodes[source_idx];

    std::cout << "DEEP COPY " << n->name() << std::endl;

    if (n->type() == ARRAY)
    {
        boost::shared_ptr<Sequence> s_new = boost::make_shared<Sequence>(n->name());
        if (target_idx == -1)
            target_idx = target.addNode(s_new, parent_idx);
        else
            target.nodes[target_idx] = s_new;

        NodeIdx previous_child_idx = -1;
        Sequence* s = static_cast<Sequence*>(n.get());
        for(std::vector<NodeIdx>::const_iterator it = s->children_.begin(); it != s->children_.end(); ++it)
        {
            NodeIdx child_idx = deepCopy(source, *it, target_idx, target);
            s_new->children_.push_back(child_idx);

            if (previous_child_idx != -1)
                target.setRightSibling(previous_child_idx, child_idx);

            previous_child_idx = child_idx;
        }
    }
    else if (n->type() == MAP)
    {
        boost::shared_ptr<Map> m_new = boost::make_shared<Map>(n->name());
        if (target_idx == -1)
            target_idx = target.addNode(m_new, parent_idx);
        else
            target.nodes[target_idx] = m_new;

        const Map* m = static_cast<const Map*>(n.get());
        const std::map<Label, NodeIdx>& children = m->map_;

        NodeIdx previous_child_idx = -1;
        for(std::map<Label, NodeIdx>::const_iterator it = children.begin(); it != children.end(); ++it)
        {
            NodeIdx child_idx = deepCopy(source, it->second, target_idx, target);
            m_new->map_[it->first] = child_idx;

            if (previous_child_idx != -1)
                target.setRightSibling(previous_child_idx, child_idx);

            previous_child_idx = child_idx;
        }
    }
    else
    {
        return -1;
    }

    return target_idx;
}

// ----------------------------------------------------------------------------------------------------

void data_merge(Data& c1, NodeIdx me_idx, const Data& c2, NodeIdx other_idx, std::string& error)
{
    NodePtr& n1 = c1.nodes[me_idx];
    const NodePtr& n2 = c2.nodes[other_idx];


    std::cout << "MERGING " << n1->name() << " and " << n2->name() << std::endl;

    if (n1->type() != n2->type())
    {
        error += "Trying to merge two nodes of different types.\n";
        return;
    }

    if (n1->type() == ARRAY)
    {
        // n1's sequence is completely replaced by n2's sequence
        deepCopy(c2, other_idx, c1.getParent(me_idx), c1, me_idx);
    } else
    {
        Map* map1 = static_cast<Map*>(n1.get());
        const Map* map2 = static_cast<const Map*>(n2.get());

        boost::shared_ptr<Map> new_map = boost::make_shared<Map>(*map1);

        // Merge children
        const std::map<Label, NodeIdx>& children = map2->map_;
        for(std::map<Label, NodeIdx>::const_iterator it2 = children.begin(); it2 != children.end(); ++it2)
        {
            const Label& label2 = it2->first;
            std::map<Label, NodeIdx>::iterator it1 = map1->map_.find(label2);
            if (it1 == map1->map_.end())
            {
                // c1 does not have that child, so add it
                new_map->map_[label2] = deepCopy(c2, it2->second, me_idx, c1);
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
            new_map->values[it2->first] = it2->second;
        }

        c1.nodes[me_idx] = new_map;
    }
}

// ----------------------------------------------------------------------------------------------------

bool DataPointer::add(const DataConstPointer& ptr)
{
    if (!data)
    {
        data = boost::make_shared<Data>(*ptr.data);
        return true;
    }

    if (!ptr.data)
        return true;

    //    std::cout << std::endl;
    //    std::cout << "----- MERGING ----" << std::endl;
    //    std::cout << *this << std::endl;
    //    std::cout << "-----  WITH   ----" << std::endl;
    //    std::cout << ptr << std::endl;

    std::string error;
    data_merge(*data, idx, *ptr.data, ptr.idx, error);

    if (!error.empty())
    {
        std::cout << "DataPointer::add: " << error << std::endl;
        return false;
    }

    //    std::cout << "------ RESULT -------" << std::endl;
    //    std::cout << *this << std::endl;
    //    std::cout << "------ ---- -------" << std::endl;

    return true;
}

// ----------------------------------------------------------------------------------------------------

std::ostream& operator<< (std::ostream& out, const DataConstPointer& d)
{
    YAMLEmitter emitter;
    emitter.emit(d, out);
    return out;
}

// ----------------------------------------------------------------------------------------------------

std::ostream& operator<< (std::ostream& out, const DataPointer& d)
{
    YAMLEmitter emitter;
    emitter.emit(d, out);
    return out;
}

} // end namespace tue

} // end namespace config

