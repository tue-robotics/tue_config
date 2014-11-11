#include "tue/config/writer.h"
#include "tue/config/node.h"
#include "tue/config/configuration.h"
#include "tue/config/map.h"
#include "tue/config/sequence.h"

#include <boost/make_shared.hpp>

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

Writer::Writer(Configuration& cfg, NodeIdx idx) : cfg_(&cfg), idx_(idx)
{
}

// ----------------------------------------------------------------------------------------------------

Writer::~Writer()
{
}

// ----------------------------------------------------------------------------------------------------

bool Writer::writeGroup(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    NodeIdx n = cfg_->addNode(boost::make_shared<Map>(label), idx_);

    if (!cfg_->nodes[idx_]->addGroup(label, n, idx_))
        return false;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Writer::writeArray(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    NodeIdx n = cfg_->addNode(boost::make_shared<Sequence>(label), idx_);

    if (!cfg_->nodes[idx_]->addGroup(label, n, idx_))
        return false;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Writer::nextArrayItem()
{
    if (cfg_->nodes[idx_]->type() != ARRAY)
        return false;

    NodePtr node = boost::make_shared<Map>(0);
    NodeIdx n = cfg_->addNode(node, idx_);

    NodeIdx previous;
    if (!cfg_->nodes[idx_]->add(n, previous))
        return false;

    if (previous != -1)
        cfg_->nodes[previous]->right_sibling = n;

    idx_ = n;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Writer::end()
{
    NodeIdx parent = cfg_->nodes[idx_]->parent;
    if (parent == -1)
        return false;

    idx_ = parent;
    return true;
}

} // end namespace tue

} // end namespace config

