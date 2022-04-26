#ifndef TUE_CONFIG_LOADERS_YAML_H_
#define TUE_CONFIG_LOADERS_YAML_H_

#include "tue/config/resolve_config.h"

#include <string>
#include <istream>

namespace tue
{

namespace config
{

/**
 * Forward declaration of ReaderWriter class
 */
class ReaderWriter;

/**
 * @brief loadFromYAMLStream loads a yaml stream into a ReaderWriter class
 * @param stream input stream
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromYAMLStream(std::istream& stream, ReaderWriter& config, const ResolveConfig& resolve_config=ResolveConfig());

/**
 * @brief loadFromYAMLString loads a yaml string into a ReaderWriter class
 * @param string input string (yaml)
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromYAMLString(const std::string& string, ReaderWriter& config, const ResolveConfig& resolve_config=ResolveConfig());

/**
 * @brief loadFromYAMLFile loads a yaml file into a ReaderWriter class
 * @param filename input file
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromYAMLFile(const std::string& filename, ReaderWriter& config, const ResolveConfig& resolve_config=ResolveConfig());

}

}

#endif
