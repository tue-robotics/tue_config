#ifndef TUE_CONFIG_LOADERS_SDF_H_
#define TUE_CONFIG_LOADERS_SDF_H_

#include <istream>
#include <string>

namespace tue
{

namespace config
{

/**
 * Forward declaration of ReaderWriter class
 */
class ReaderWriter;

/**
 * @brief loadFromSDFStream loads a sdf stream into a ReaderWriter class
 * @param stream input stream
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromSDFStream(std::istream& stream, ReaderWriter& config);

/**
 * @brief loadFromSDFString loads a sdf string into a ReaderWriter class
 * @param string input string (sdf)
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromSDFString(const std::string& string, ReaderWriter& config);

/**
 * @brief loadFromSDFFile loads a sdf file into a ReaderWriter class
 * @param filename input file
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromSDFFile(const std::string& filename, ReaderWriter& config);

}  // End of namespace config

}  // End of namespace tue

#endif // TUE_CONFIG_LOADERS_SDF_H_
