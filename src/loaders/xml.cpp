#include <fstream>
#include <sstream>
#include <tinyxml2.h>

#include "tue/config/configuration.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"
#include "loader_functions.h"

#include "tue/filesystem/path.h"

namespace tue
{

namespace config
{

bool setValue(const std::string& key, const std::string& value, ReaderWriter& config)
{
    char* pEnd;
    int i = (int) std::strtol(value.c_str(), &pEnd, 10);
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

    bool b = false;
    if (strToBool(value, b))
    {
        config.setValue(key, b);
        return true;
    }

    config.setValue(key, value);
    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLText(const tinyxml2::XMLElement& element, ReaderWriter& config)
{
    std::string key(element.Value());
    if (element.GetText() == nullptr)
    {
        std::cout << "Empty key: " << key << std::endl;
        return setValue(key, "", config);
    }
    std::string value(element.GetText());
    return setValue(key, value, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLElement(const tinyxml2::XMLElement& element, ReaderWriter& config)
{
    // Attributes aren't read, if element doesn't have any child elements
    if (element.FirstChildElement() == nullptr)
    {
        return loadFromXMLText(element, config);
    }
    else
    {
        // Start a new array with the Value of the current element as key
        std::string element_name = element.Value();
        config.writeArray(element_name);

        // Iterate through attributes
        // if this element does not contain children, we don't end up here
        for (const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
        {
            config.addArrayItem();
            setValue(attribute->Name(), attribute->Value(), config);
            config.endArrayItem();
        }

        // Iterate through elements
        for(const tinyxml2::XMLElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            std::string candidate_name = e->Value();
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

bool loadFromXMLDocument(const tinyxml2::XMLDocument& doc, ReaderWriter& config)
{
    const tinyxml2::XMLElement* root = doc.FirstChildElement();

    if (root->NextSibling() != nullptr)
    {
        throw tue::config::ParseException("A valid XML file should only contain one root element");
    }

    return loadFromXMLElement(*root, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLStream(std::istream& stream, ReaderWriter& config)
{
    tinyxml2::XMLDocument doc;
    std::string stream_string;
    stream >> stream_string;
    doc.Parse(stream_string.c_str());
    if (doc.Error())
    {
        std::stringstream error;
        error << "Error loading stream:" << std::endl << stream.rdbuf() << std::endl;
        error << doc.ErrorStr() << " at line " << doc.ErrorLineNum();
        config.addError(error.str());
        return false;
    }
    return loadFromXMLDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLString(const std::string& string, ReaderWriter& config)
{
    tinyxml2::XMLDocument doc;
    doc.Parse(string.c_str());
    if (doc.Error())
    {
        std::stringstream error;
        error << "Error loading string:" << std::endl << string << std::endl;
        error << doc.ErrorStr() << " at line " << doc.ErrorLineNum();
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
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());
    if (doc.Error())
    {
        std::stringstream error;
        error << "Error loading " << filename << ": ";
        error << doc.ErrorStr() << " at row " << doc.ErrorLineNum();
        config.addError(error.str());
        return false;
    }

    // read the document
    return loadFromXMLDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

}  // End of namespace config

}  // End of namespace tue
