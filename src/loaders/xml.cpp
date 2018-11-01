#include <fstream>
#include <sstream>
#include <set>
#include <tinyxml.h>

#include "tue/config/configuration.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"
#include "tue/config/node_type.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

tue::config::NodeType getNodeType (const std::set<std::string>& array_elements, const std::string& element_name)
{
    std::set<std::string>::const_iterator it = array_elements.find(element_name);
    if (it != array_elements.end())
        return tue::config::ARRAY;

    return tue::config::MAP;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLText(const TiXmlElement& element, ReaderWriter& config)
{
    std::string key(element.ValueStr());
    if (element.GetText() == nullptr)
    {
        std::cout << "Skipping " << key << std::endl;
        return true;
    }
    std::string value(element.GetText());

    // parsing to int/double
    char* pEnd;

    int i = std::strtol(value.c_str(), &pEnd, 10);
    if (pEnd[0] == 0)
    {
        config.setValue(key, i);
        return true;
    }

    double d = std::strtod(value.c_str(), &pEnd);
    if (pEnd[0] == 0)
    {
        config.setValue(key, d);
        return true;
    }

    config.setValue(key, value);
    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLElement(const TiXmlElement& element, ReaderWriter& config, tue::config::NodeType node_type = tue::config::MAP)
{
    if (element.FirstChildElement() == nullptr)
    {
        return loadFromXMLText(element, config);
    }
    else
    {
        // Check which child elements are occuring multiple times. These should be added as elements to an array.
        std::set<std::string> child_elements, array_elements;
        for (const TiXmlElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            if(!child_elements.insert(e->ValueStr()).second) // When inserting fails, value is already in set. So that child has to be an array item.
            {
                array_elements.insert(e->ValueStr());
            }
        }

        // Start new group or array item
        // in case of array the array is opened on parent level. Only need to add array items on this level.
        if (node_type == tue::config::ARRAY)
            config.addArrayItem();
        else
            // Start a new group
            config.writeGroup(element.ValueStr());


        // Iterate through attributes
        // ToDo: this does not work if this element does not contain children (we don't end up here)
        for (const TiXmlAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
        {
            double d;
            if (attribute->QueryDoubleValue(&d) == TIXML_SUCCESS)
            {
                config.setValue(attribute->NameTStr(), d);
                continue;
            }
            int i;
            if (attribute->QueryIntValue(&i) == TIXML_SUCCESS)
            {
                config.setValue(attribute->NameTStr(), i);
                continue;
            }
            config.setValue(attribute->NameTStr(), attribute->ValueStr());
        }

        // Iterate through elements
        for(const TiXmlElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            std::string candidate_name = e->ValueStr();
            tue::config::NodeType candidate_node_type = getNodeType(array_elements, candidate_name);
            // Because potentially childs of same type could be read, interupted by an other type. Hence opening and closing of the array is need for each child.
            // First try to read the array. If not yet created, write a new one. (writing never fails at the moment)
            if (candidate_node_type == tue::config::ARRAY && !(config.readArray(candidate_name, tue::REQUIRED) || config.writeArray(candidate_name)))
            {
                std::stringstream error;
                error << "Could not write or read array: " << candidate_name;
                config.addError(error.str());
                std::cout << error.str() << std::endl;
                return false;
            }

            // load child element
            if (!loadFromXMLElement(*e, config, candidate_node_type))
            {
                std::stringstream error;
                error << "Error parsing " << candidate_name;
                config.addError(error.str());
                std::cout << error.str() << std::endl;
                return false;
            }

            //
            if (candidate_node_type == tue::config::ARRAY)
                config.endArray();
        }
        // Ending array item or group
        if (node_type == tue::config::ARRAY)
            config.endArrayItem();
        else
            config.endGroup();
    }

    return true;

}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLDocument(const TiXmlDocument& doc, ReaderWriter& config)
{
    const TiXmlElement* root = doc.FirstChildElement();

    if (root->NextSibling() != nullptr)
    {
        throw tue::config::ParseException("A valid XML file should only contain one root element");
    }

    return loadFromXMLElement(*root, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLStream(std::istream& stream, ReaderWriter& config)
{
    TiXmlDocument doc;
    stream >> doc;
    if (doc.Error())
    {
        std::stringstream error;
        error << "Error loading stream:" << std::endl << stream.rdbuf() << std::endl;
        error << doc.ErrorDesc() << " at row " << doc.ErrorRow() << ", col " << doc.ErrorCol();
        config.addError(error.str());
        return false;
    }
    return loadFromXMLDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLString(const std::string& string, ReaderWriter& config)
{
    TiXmlDocument doc;
    doc.Parse(string.c_str());
    if (doc.Error())
    {
        std::stringstream error;
        error << "Error loading string:" << std::endl << string << std::endl;
        error << doc.ErrorDesc() << " at row " << doc.ErrorRow() << ", col " << doc.ErrorCol();
        config.addError(error.str());
        return false;
    }
    return loadFromXMLDocument(doc, config);
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
        std::stringstream error;
        error << "Error loading " << filename << ": ";
        error << doc.ErrorDesc() << " at row " << doc.ErrorRow() << ", col " << doc.ErrorCol();
        config.addError(error.str());
        return false;
    }

    // read the document
    return loadFromXMLDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

}  // End of namespace config

}  // End of namespace tue
