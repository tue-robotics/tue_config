#include <console_bridge/console.h>

#include <cstdlib>
#include <istream>
#include <sstream>
#include <string>
#include <tinyxml2.h>

#include "loader_functions.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"
#include "tue/config/reader_writer.h" // IWYU pragma: keep — complete type needed for ReaderWriter method calls

#include <filesystem>

namespace tue::config
{

bool setValue(const std::string& key, const std::string& value, ReaderWriter& config)
{
    // strtol writes the end pointer through &p_end, so its pointee cannot be const (the C API
    // signature is char**), even though we never modify the pointed-to characters ourselves.
    char* p_end = nullptr; // NOLINT(misc-const-correctness)
    const int i = static_cast<int>(std::strtol(value.c_str(), &p_end, 10));
    if (p_end[0] == 0)
    {
        config.setValue(key, i);
        return true;
    }

    const double d = std::strtod(value.c_str(), &p_end);
    if (p_end[0] == 0)
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
    const std::string key(element.Value());
    if (element.GetText() == nullptr)
    {
        CONSOLE_BRIDGE_logWarn("Empty key: %s", key.c_str());
        return setValue(key, "", config);
    }
    const std::string value(element.GetText());
    return setValue(key, value, config);
}

// ----------------------------------------------------------------------------------------------------

namespace
{

// NOLINTNEXTLINE(misc-no-recursion) — recursive descent over nested XML elements.
bool loadFromXMLElement(const tinyxml2::XMLElement& element, ReaderWriter& config)
{
    // Attributes aren't read, if element doesn't have any child elements
    if (element.FirstChildElement() == nullptr)
    {
        return loadFromXMLText(element, config);
    }

    {
        // Start a new array with the Value of the current element as key
        const std::string element_name = element.Value();
        config.writeArray(element_name);

        // Iterate through attributes
        // if this element does not contain children, we don't end up here
        for (const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr;
             attribute = attribute->Next())
        {
            config.addArrayItem();
            setValue(attribute->Name(), attribute->Value(), config);
            config.endArrayItem();
        }

        // Iterate through elements
        for (const tinyxml2::XMLElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            const std::string candidate_name = e->Value();
            config.addArrayItem();

            if (!loadFromXMLElement(*e, config))
            {
                std::stringstream error;
                error << "Error parsing " << candidate_name;
                config.addError(error.str());
                CONSOLE_BRIDGE_logError("%s", error.str().c_str());
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

} // namespace

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
        error << "Error loading stream:" << '\n' << stream.rdbuf() << '\n';
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
        error << "Error loading string:" << '\n' << string << '\n';
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

    if (!std::filesystem::exists(filename))
    {
        std::stringstream error;
        error << "[loadFromXMLFile] file '" << filename << "' doesn't exist." << '\n';
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

} // namespace tue::config
