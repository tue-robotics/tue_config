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

    NodeIdx parent = cfg_->nodes[idx_]->parent;
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

    NodeIdx right_sibling = cfg_->nodes[idx_]->right_sibling;
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
            const std::string& name2 = c2.getName(it2->second);


            Label label1;
            if (c1.getLabel(name2, label1))
            {
                std::map<std::string, NodeIdx>::iterator it1 = map1->map_.find(label1);
                if (it1 == map1->map_.end())
                {
                    c1.nodes[it1->second] = n2;

                    // n1 does not have that child. Copy it and add it to n1
                    ConfigNodePtr c(new ConfigNode(*it2->second));
                    c->parent = n1;
                    n1->children[name2] = c;
                }
            }



            else
            {
                // n1 already has that child. Merge them.
                merge(it1->second.get(), *it2->second, error);
            }
        }

        // Merge values
        const std::map<Label, Variant>& values = map->values;
        for(std::map<Label, Variant>::const_iterator it2 = values.begin(); it2 != values.end(); ++it2)
        {

        }


    }







    // Merge children
    for(std::map<std::string, ConfigNodePtr>::const_iterator it2 = n2.children.begin(); it2 != n2.children.end(); ++it2)
    {
        const std::string& name = it2->first;
        std::map<std::string, ConfigNodePtr>::iterator it1 = n1->children.find(name);
        if (it1 == n1->children.end())
        {
            // n1 does not have that child. Copy it and add it to n1
            ConfigNodePtr c(new ConfigNode(*it2->second));
            c->parent = n1;
            n1->children[name] = c;
        }
        else
        {
            // n1 already has that child. Merge them.
            merge(it1->second.get(), *it2->second, error);
        }
    }

    // Merge values
    for(std::map<std::string, tue::Variant>::const_iterator it2 = n2.values.begin(); it2 != n2.values.end(); ++it2)
    {
        n1->values[it2->first] = it2->second;
    }

    // n1's sequence is completely replaced by n2's sequence
    n1->sequence.resize(n2.sequence.size());
    for(unsigned int i = 0; i < n2.sequence.size(); ++i)
    {
        ConfigNodePtr c(new ConfigNode(*n2.sequence[i]));
        c->parent = n1;
        n1->sequence[i] = c;
    }
}

// ----------------------------------------------------------------------------------------------------

void ReaderWriter::add(const ReaderWriter& rw)
{
    std::string error;
    merge(*this, idx_, rw, rw.idx_, error);

    if (!error.empty())
    {
        addError(error);
        return false;
    }

    return true;
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

