#ifndef TUE_CONFIG_CONFIG_DATA_H_
#define TUE_CONFIG_CONFIG_DATA_H_

#include "config_set.h"

struct ConfigData
{
    ConfigData() : root("") {}

    ConfigSet root;
    std::string error;
};

#endif
