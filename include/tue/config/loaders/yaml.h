#ifndef TUE_CONFIG_LOADERS_YAML_H_
#define TUE_CONFIG_LOADERS_YAML_H_

#include <string>
#include <istream>

class ConfigData;
class ConfigNode;

namespace tue
{

class Configuration;

namespace config
{

bool loadFromYAMLStream(std::istream& stream, tue::Configuration& config);

bool loadFromYAMLString(const std::string& string, tue::Configuration& config);

bool loadFromYAMLFile(const std::string& filename, tue::Configuration& config);

}

}

#endif
