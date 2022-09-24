#include "tue/config/map.h"
#include "tue/config/data.h"

namespace tue
{

namespace config
{

NodePtr Map::deepCopy(const Data& source, NodeIdx target_idx, Data& target) const
{
    boost::shared_ptr<Map> n(new Map(name()));

    NodeIdx previous_child_idx = -1;
    for(std::map<Label, NodeIdx>::const_iterator it = map_.begin(); it != map_.end(); ++it)
    {
        NodeIdx child_idx = target.addNode(NodePtr(), target_idx);
        target.nodes[child_idx] = source.nodes[it->second]->deepCopy(source, child_idx, target);
        n->map_[it->first] = child_idx;

        if (previous_child_idx != static_cast<NodeIdx>(-1))
            target.setRightSibling(previous_child_idx, child_idx);

        previous_child_idx = child_idx;
    }

    // VALUES
    n->values = values;

    return n;
}

}

} // end namespace tue

