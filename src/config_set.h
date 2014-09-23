#ifndef TUE_CONFIG_CONFIG_SET_H_
#define TUE_CONFIG_CONFIG_SET_H_

#include <map>
#include <string>

#include "tue/config/variant.h"

#include <boost/shared_ptr.hpp>

class ConfigNode;
typedef boost::shared_ptr<ConfigNode> ConfigNodePtr;

struct ConfigNode
{
    ConfigNode(const std::string& name_) : name(name_), parent(0), sequence_parent(0), is_array(false), idx_array(0) {}

    std::string name;

    ConfigNode* parent;
    ConfigNode* sequence_parent;
    bool is_array;
    int idx_array;
    std::map<std::string, ConfigNodePtr> children;
    std::map<std::string, tue::Variant> values;
    std::vector<ConfigNodePtr> sequence;
};

#endif
