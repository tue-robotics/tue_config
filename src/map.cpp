#include "tue/config/map.h"
#include "tue/config/data.h"
#include "tue/config/data_pointer.h"
#include "tue/config/types.h"
#include <boost/smart_ptr/shared_ptr.hpp>

namespace tue::config
{

NodePtr Map::deepCopy(const Data& source, NodeIdx target_idx, Data& target) const
{
    boost::shared_ptr<Map> n(new Map(name()));

    NodeIdx previous_child_idx = -1;
    for (const auto& it : map_)
    {
        NodeIdx const child_idx = target.addNode(NodePtr(), target_idx);
        target.nodes[child_idx] = source.nodes[it.second]->deepCopy(source, child_idx, target);
        n->map_[it.first] = child_idx;

        if (previous_child_idx != static_cast<NodeIdx>(-1))
            target.setRightSibling(previous_child_idx, child_idx);

        previous_child_idx = child_idx;
    }

    // VALUES
    n->values = values;

    return n;
}

} // namespace tue::config
