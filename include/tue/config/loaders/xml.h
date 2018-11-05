#ifndef TUE_CONFIG_LOADERS_XML_H_
#define TUE_CONFIG_LOADERS_XML_H_

#include <istream>
#include <string>
#include <tinyxml.h>

namespace tue
{

namespace config
{

/**
 * Forward declaration of ReaderWriter class
 */
class ReaderWriter;

/**
 * @brief loadFromXMLText writes a value from xml element into config object.
 * Stored as double, int or string
 * @param element TinyXML element
 * @param config object in which the data is stored
 * @return indicates succes
 */
bool loadFromXMLText(const TiXmlElement& element, ReaderWriter& config);

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
