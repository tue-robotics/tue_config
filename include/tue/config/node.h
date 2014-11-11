#ifndef TUE_CONFIG2_NODE_H_
#define TUE_CONFIG2_NODE_H_

#include "tue/config2/types.h"
#include "tue/config2/variant.h"
#include "tue/config2/label.h"

#include <map>

#include <boost/weak_ptr.hpp>

namespace tue
{

namespace config
{

enum NodeType
{
    MAP,
    ARRAY
};

class Pointer;

class Iterator
{

public:

    virtual NodePtr get() const = 0;
    virtual void incr() const = 0;
    virtual bool isEnd() const = 0;
};

class Node
{

public:

    Node(const Label& name, NodeType type) : parent(-1), right_sibling(-1), name_(name), type_(type) {}

    virtual ~Node() {}

    // READ-ONLY

    // --- MAP

    virtual bool readGroup(const Label& label, NodeIdx& idx) const { return false; }

    virtual bool value(const Label& label, Variant& value) const { return false; }


    // WRITE

    // --- MAP

    virtual bool addGroup(const Label& label, const NodeIdx& n, NodeIdx& idx) { return false; }

    virtual bool setValue(const Label& label, const Variant& value) { return false; }

    // --- ARRAY

    virtual bool add(const NodeIdx& n, NodeIdx& previous) { return false; }

    virtual bool firstChild(NodeIdx& child) { return false; }


    inline const Label& name() const { return name_; }

    inline const NodeType& type() const { return type_; }

    NodeIdx parent;
    NodeIdx right_sibling;

private:

    Label name_;

    NodeType type_;

};

}

} // end namespace tue

#endif
