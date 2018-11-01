#ifndef TUE_CONFIG_YAML_EMITTER_H_
#define TUE_CONFIG_YAML_EMITTER_H_

#include "tue/config/types.h"

#include <string>

namespace tue
{

namespace config
{

class Data;

class YAMLEmitter
{

public:

    YAMLEmitter();

    void emit(const tue::config::DataConstPointer& cfg, std::ostream& out, const std::string& indent = "");

};

} // end namespace config

} // end namespace tue

#endif
