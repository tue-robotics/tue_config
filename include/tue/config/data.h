#ifndef TUE_CONFIG2_CONFIGURATION_H_
#define TUE_CONFIG2_CONFIGURATION_H_

#include "tue/config/types.h"

#include <map>
#include <vector>
#include "tue/config/label.h"
#include "tue/config/node.h"

namespace tue
{

namespace config
{

class Data
{

public:

    Data();

    virtual ~Data();

    std::map<std::string, Label> name_to_label;

    std::vector<std::string> label_to_name;

    Label getOrAddLabel(const std::string& name)
    {
        return name;
//        std::map<std::string, Label>::const_iterator it = name_to_label.find(name);
//        if (it != name_to_label.end())
//            return it->second;

//        Label l = name_to_label.size();
//        name_to_label[name] = l;
//        label_to_name.push_back(name);
//        return l;
    }

    bool getLabel(const std::string& name, Label& label) const
    {
//        std::map<std::string, Label>::const_iterator it = name_to_label.find(name);
//        if (it == name_to_label.end())
//            return false;

//        label = it->second;
//        return true;
        label = name;
        return true;
    }

    const std::string& getName(const Label& label) const
    {
//        return label_to_name[label];
        return label;
    }

    NodeIdx addNode(const NodePtr& n, NodeIdx parent)
    {
        parents.push_back(parent);
        nodes.push_back(n);
        right_siblings.push_back(-1);
        return nodes.size() - 1;
    }

    inline NodeIdx root() const { return 0; }

    inline NodeIdx getParent(NodeIdx n) const { return parents[n]; }
    inline NodeIdx getRightSibling(NodeIdx n) const { return right_siblings[n]; }

    inline void getParent(NodeIdx n, NodeIdx parent) { parents[n] = parent; }
    inline void setRightSibling(NodeIdx n, NodeIdx sibling) { right_siblings[n] = sibling; }

    std::vector<NodePtr> nodes;
    std::vector<NodeIdx> parents;
    std::vector<NodeIdx> right_siblings;

//    friend std::ostream& operator<< (std::ostream& out, const Data& d);

};

}

} // end namespace tue

#endif
