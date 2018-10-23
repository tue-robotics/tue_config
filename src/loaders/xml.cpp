#include <fstream>
#include <sstream>
#include <tinyxml.h>

#include "tue/config/configuration.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLStream(std::istream& stream, ReaderWriter& config)
{
  throw tue::config::ParseException("loadFromXMLStream is not yet implemented");
  return false;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLString(const std::string& string, ReaderWriter& config)
{
  throw tue::config::ParseException("loadFromXMLString is not yet implemented");
  return false;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLText(TiXmlElement& element, ReaderWriter& config)
{
  std::string key(element.Value());
  if (element.GetText() == NULL)
  {
    std::cout << "Skipping " << element.Value() << std::endl;
    return true;
  }
  std::string value(element.GetText());
  config.setValue(key, value);
  return true;
}

// ----------------------------------------------------------------------------------------------------

// ToDo: make TiXmlElement const
bool loadFromXMLElement(TiXmlElement& element, ReaderWriter& config)
{
  if (element.FirstChildElement() == NULL)
  {
    std::cout << "Parsing text " << std::endl;
    return loadFromXMLText(element, config);
  }
  else
  {
    std::cout << "Parsing element " << std::endl;

    // Start a new array with the Value of the current element as key
    std::string element_name = element.Value();
    config.writeArray(element_name);

    // Iterate through attributes
    // ToDo: this does not work if this element does not contain children (we don't end up here)
    for (const TiXmlAttribute* attribute = element.FirstAttribute(); attribute != NULL; attribute = attribute->Next())
    {
        config.addArrayItem();
        config.setValue(attribute->Name(), attribute->Value());
    }

    // Recurse through models
    for(TiXmlElement* e = element.FirstChildElement(); e != NULL; e = e->NextSiblingElement())
    {
      std::string candidate_name = e->Value();
      config.addArrayItem();

      if (!loadFromXMLElement(*e, config))
      {
        std::stringstream error_msg;
        error_msg << "Error parsing " << e->Value();
        config.addError(error_msg.str());
        std::cout << error_msg.str() << std::endl;
        return false;
      }
      config.endArrayItem();
    }
    config.endArray();
  }

  return true;

}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLFile(const std::string& filename, ReaderWriter& config)
{
  config.setSource(filename);

//  std::ifstream fin(filename.c_str());
//  if (fin.fail())
//  {
//      config.addError("No such file: '" + filename + "'.");
//      return false;
//  }

//  if (!loadFromXMLStream(fin, config))
//      return false;

  // Load the file
  TiXmlDocument doc(filename);
  doc.LoadFile();
  if (doc.Error())
  {
    std::stringstream error_msg;
    error_msg << "Error loading " << filename << ": ";
    error_msg << doc.ErrorDesc() << " at row " << doc.ErrorRow() << ", col " << doc.ErrorCol();
    config.addError(error_msg.str());
    return false;
  }

  // Get the root
  // ToDo: only sdf?
  TiXmlElement* root = doc.FirstChildElement();

  if (root->NextSibling() != NULL)
  {
    throw tue::config::ParseException("A valid XML file should only contain one root element");
  }

//  std::string key("model");
  return loadFromXMLElement(*root, config);

}

// ----------------------------------------------------------------------------------------------------

}  // End of namespace config

}  // End of namespace tue
