#include "tue/config/writer.h"
#include "tue/config/data.h"
#include "tue/config/map.h"
#include "tue/config/node.h"
#include "tue/config/sequence.h"

#include <boost/make_shared.hpp>

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

Writer::Writer() : idx_(0)
{
    cfg_.reset(new Data);
}

// ----------------------------------------------------------------------------------------------------

Writer::Writer(DataPointer& cfg) : idx_(cfg.idx), cfg_(cfg.data)
{
    if (!cfg_)
    {
        cfg_.reset(new Data);
        cfg.data = cfg_;
    }
}

// ----------------------------------------------------------------------------------------------------

Writer::~Writer() = default;

// ----------------------------------------------------------------------------------------------------

bool Writer::writeGroup(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    if (cfg_->nodes[idx_]->readGroup(label, idx_))
        return true;

    // If no child node with name is known, create a new one.
    const NodeIdx n = cfg_->addNode(boost::make_shared<Map>(label), idx_);

    return cfg_->nodes[idx_]->addGroup(label, n, idx_);
}

// ----------------------------------------------------------------------------------------------------

bool Writer::writeArray(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    if (cfg_->nodes[idx_]->readGroup(label, idx_))
        return true;

    // If no child node with name is known, create a new one.
    const NodeIdx n = cfg_->addNode(boost::make_shared<Sequence>(label), idx_);

    return cfg_->nodes[idx_]->addGroup(label, n, idx_);
}

// ----------------------------------------------------------------------------------------------------

bool Writer::addArrayItem()
{
    if (cfg_->nodes[idx_]->type() != ARRAY)
        return false;

    const NodePtr node = boost::make_shared<Map>("");
    const NodeIdx n = cfg_->addNode(node, idx_);

    NodeIdx previous = 0;
    if (!cfg_->nodes[idx_]->add(n, previous))
        return false;

    if (previous != static_cast<NodeIdx>(-1))
        cfg_->setRightSibling(previous, n);

    idx_ = n;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Writer::end()
{
    const NodeIdx parent = cfg_->getParent(idx_);
    if (parent == static_cast<NodeIdx>(-1))
        return false;

    idx_ = parent;

    // If the parent is an array, go up one more
    if (cfg_->nodes[idx_]->type() == ARRAY)
        idx_ = cfg_->getParent(idx_);

    return true;
}

} // namespace tue::config
