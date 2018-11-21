#include <fstream>
#include <sstream>
#include <tinyxml.h>

#include "tue/config/configuration.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"

#include "tue/filesystem/path.h"

namespace tue
{

namespace config
{

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

bool loadFromXMLElement(const TiXmlElement& element, ReaderWriter& config)
{
    // Attributes aren't read, if element doesn't have any child elements
    if (element.FirstChildElement() == nullptr)
    {
        return loadFromXMLText(element, config);
    }
    else
    {
        // Start a new array with the Value of the current element as key
        std::string element_name = element.ValueStr();
        config.writeArray(element_name);

        // Iterate through attributes
        // if this element does not contain children, we don't end up here
        for (const TiXmlAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
        {
            int i;
            if (attribute->QueryIntValue(&i) == TIXML_SUCCESS)
            {
                config.setValue(attribute->NameTStr(), i);
                continue;
            }
            double d;
            if (attribute->QueryDoubleValue(&d) == TIXML_SUCCESS)
            {
                config.setValue(attribute->NameTStr(), d);
                continue;
            }
            config.setValue(attribute->NameTStr(), attribute->ValueStr());
        }

        // Iterate through elements
        for(const TiXmlElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            std::string candidate_name = e->ValueStr();
            config.addArrayItem();

            if (!loadFromXMLElement(*e, config))
            {
                std::stringstream error;
                error << "Error parsing " << candidate_name;
                config.addError(error.str());
                std::cout << error.str() << std::endl;
                return false;
            }
            config.endArrayItem();
        }
        config.endArray();
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

    tue::filesystem::Path path(filename);
    if (!path.exists())
    {
        std::stringstream error;
        error << "[loadFromXMLFile] file '" << filename << "' doesn't exist." << std::endl;
        config.addError(error.str());
        return false;
    }

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
