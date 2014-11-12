#include "tue/config/reader_writer.h"
#include "tue/config/node.h"
#include "tue/config/data.h"

// Merge
#include "tue/config/sequence.h"
#include "tue/config/map.h"

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

ReaderWriter::ReaderWriter() : idx_(0), scope_(0), cfg_(new Configuration)
{
}

// ----------------------------------------------------------------------------------------------------

ReaderWriter::~ReaderWriter()
{
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::read(const std::string& name)
{
    Label label;
    if (cfg_->getLabel(name, label) && cfg_->nodes[idx_]->readGroup(label, idx_))
        return true;
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::end()
{   
    if (idx_ == scope_)
        return false;

    NodeIdx parent = cfg_->getParent(idx_);
    if (parent == -1)
        return false;

    idx_ = parent;
    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::next()
{
    if (cfg_->nodes[idx_]->type() == ARRAY)
        return cfg_->nodes[idx_]->firstChild(idx_);

    NodeIdx right_sibling = cfg_->getRightSibling(idx_);
    if (right_sibling == -1)
        return false;

    idx_ = right_sibling;
    return true;
}

// ----------------------------------------------------------------------------------------------------

ReaderWriter ReaderWriter::limitScope() const
{
    ReaderWriter r(*this);
    r.scope_ = r.idx_;
    return r;
}

// ----------------------------------------------------------------------------------------------------

void merge(Configuration& c1, NodeIdx& me_idx, const Configuration& c2, NodeIdx other_idx, std::string& error)
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
                merge(c1, it1->second, c2, it2->second, error);
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

bool ReaderWriter::add(const ReaderWriter& rw)
{
    std::string error;
    merge(*this->cfg_, idx_, *rw.cfg_, rw.idx_, error);

    if (!error.empty())
    {
        addError(error);
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

void ReaderWriter::addError(const std::string& msg)
{
    // build context
    std::vector<std::string> context;

    NodeIdx c = idx_;
    while(c != -1)
    {
        const NodePtr& n = cfg_->nodes[c];
        context.push_back(n->name());
        c = cfg_->getParent(c);
    }

    if (context.size() > 1)
    {
        error_ += "In '";

        for(int i = context.size() - 2; i > 0; --i)
        {
            error_ += context[i] + ".";
        }

        error_ += context[0] + "': \n\n";
    }
    else
    {
        error_ += "In root of configuration:\n\n";
    }

    error_ += "    " + msg + "\n\n";
}

// ----------------------------------------------------------------------------------------------------

void ReaderWriter::print() const
{
//    std::cout << "POINTER: [ ";
//    for(unsigned int i = 0; i < ptr_.stack.size(); ++i)
//        std::cout << ptr_.stack[i] << " ";
//    std::cout << "]" << std::endl;
}

} // end namespace config

} // end namespace tue

