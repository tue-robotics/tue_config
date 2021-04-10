#ifndef TUE_CONFIG_LOADERS_XML_H_
#define TUE_CONFIG_LOADERS_XML_H_

#include <istream>
#include <string>
#include <tinyxml2.h>

namespace tue
{

namespace config
{

/**
 * Forward declaration of ReaderWriter class
 */
class ReaderWriter;

/**
 * @brief setValue sets the value as a double, int or string
 * @param key name of the node
 * @param value value of the node
 * @param config ReaderWriter object to add it to
 * @return indicates succes
 */
bool setValue(const std::string& key, const std::string& value, ReaderWriter& config);

/**
 * @brief loadFromXMLText writes a value from xml element into config object.
 * Stored as double, int or string
 * @param element TinyXML2 element
 * @param config object in which the data is stored
 * @return indicates succes
 */
bool loadFromXMLText(const tinyxml2::XMLElement& element, ReaderWriter& config);

/**
 * @brief loadFromXMLStream loads a xml stream into a ReaderWriter class
 * @param stream input stream
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromXMLStream(std::istream& stream, ReaderWriter& config);

/**
 * @brief loadFromXMLString loads a xml string into a ReaderWriter class
 * @param string input string (xml)
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromXMLString(const std::string& string, ReaderWriter& config);

/**
 * @brief loadFromXMLFile loads a xml file into a ReaderWriter class
 * @param filename input file
 * @param config object in which the data is stored
 * @return indicates success
 */
bool loadFromXMLFile(const std::string& filename, ReaderWriter& config);

}  // End of namespace config

}  // End of namespace tue

#endif // TUE_CONFIG_LOADERS_XML_H_
