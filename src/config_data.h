#ifndef TUE_CONFIG_CONFIG_DATA_H_
#define TUE_CONFIG_CONFIG_DATA_H_

#include "config_node.h"

struct ConfigData
{
    ConfigData() : root("") {}

    ConfigNode root;
    std::string error;
};

#endif
