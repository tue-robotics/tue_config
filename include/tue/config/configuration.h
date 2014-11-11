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

class Configuration
{

public:

    Configuration();

    virtual ~Configuration();

    std::map<std::string, Label> name_to_label;

    std::vector<std::string> label_to_name;

    Label getOrAddLabel(const std::string& name)
    {
        std::map<std::string, Label>::const_iterator it = name_to_label.find(name);
        if (it != name_to_label.end())
            return it->second;

        Label l = name_to_label.size();
        name_to_label[name] = l;
        label_to_name.push_back(name);
        return l;
    }

    bool getLabel(const std::string& name, Label& label) const
    {
        std::map<std::string, Label>::const_iterator it = name_to_label.find(name);
        if (it == name_to_label.end())
            return false;

        label = it->second;
        return true;
    }

    const std::string& getName(const Label& label) const
    {
        return label_to_name[label];
    }

    NodeIdx addNode(const NodePtr& n, NodeIdx parent)
    {
        n->parent = parent;
        nodes.push_back(n);
        return nodes.size() - 1;
    }

    std::vector<NodePtr> nodes;

};

}

} // end namespace tue

#endif
