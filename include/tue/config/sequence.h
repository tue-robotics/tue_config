#ifndef TUE_CONFIG2_SEQUENCE_H_
#define TUE_CONFIG2_SEQUENCE_H_

#include "node.h"

#include <vector>

namespace tue
{

namespace config
{

class Sequence : public Node
{

public:

    Sequence(const Label& name) : Node(name, ARRAY) {}

    bool add(const NodeIdx& n, NodeIdx& previous)
    {
        if (children_.empty())
            previous = -1;
        else
            previous = children_.back();

        children_.push_back(n);
        return true;
    }

    bool firstChild(NodeIdx& child)
    {
        if (children_.empty())
            return false;
        child = children_.front();
        return true;
    }

    std::vector<NodeIdx> children_;

    // --- COPY

    NodePtr deepCopy(const Data& source, NodeIdx target_idx, Data& target) const;


};

}

} // end namespace tue

#endif
