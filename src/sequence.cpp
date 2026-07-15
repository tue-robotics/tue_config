#include "tue/config/sequence.h"
#include "tue/config/data.h"
#include "tue/config/data_pointer.h"
#include "tue/config/types.h"
#include <boost/smart_ptr/shared_ptr.hpp>

namespace tue::config
{

NodePtr Sequence::deepCopy(const Data& source, NodeIdx target_idx, Data& target) const
{
    boost::shared_ptr<Sequence> n(new Sequence(name()));

    n->children_.resize(children_.size());
    for (unsigned int i = 0; i < children_.size(); ++i)
    {
        NodeIdx const child_idx = target.addNode(NodePtr(), target_idx);
        target.nodes[child_idx] = source.nodes[children_[i]]->deepCopy(source, child_idx, target);
        n->children_[i] = child_idx;

        if (i > 0)
            target.setRightSibling(n->children_[i - 1], child_idx);
    }

    return n;
}

} // namespace tue::config
