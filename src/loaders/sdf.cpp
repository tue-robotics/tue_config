#include <fstream>
#include <sstream>
#include <set>
#include <tinyxml.h>

#include "tue/config/configuration.h"
#include "tue/config/loaders/sdf.h"
#include "tue/config/loaders/xml.h"
#include "tue/config/read.h"
#include "tue/config/node_type.h"

#include "tue/filesystem/path.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------
// all tags which should be an array(list)
static const std::set<std::string> SDF_ARRAY_SET {"include", "link", "model" , "collision", "visual", "virtual_volume", "joint"};
// all tags which should be an group(dict)
static const std::set<std::string> SDF_MAP_SET {"sdf", "world", "audio", "wind", "pose", "atmosphere", "gui", "camera",
                                                "track_visual", "plugin", "gripper", "geometry", "box",
                                                "cylinder", "heightmap", "blend", "image", "mesh", "texture", "plane",
                                                "polyline", "sphere" };
// all tags which should be an value
static const std::set<std::string> SDF_VALUE_SET {"device", "linear_velocity", "uri", "name", "static", "pose", "gravity",
                                            "magnetic_field", "type", "temperature", "pressure", "temperature_gradient",
                                            "fullscreen", "view_controller", "projection_type", "min_dist", "max_dist",
                                            "use_model_frame", "xyz", "inherit_yaw", "filename", "empty", "size",
                                            "radius", "length", "pos", "diffuse", "normal", "min_height", "fade_dist",
                                            "sampling", "point", "height" };


// ----------------------------------------------------------------------------------------------------

tue::config::NodeType getSDFNodeType(const std::string& element_name)
{
    // check if the element name is in the array set
    std::set<std::string>::const_iterator it = SDF_ARRAY_SET.find(element_name);
    if (it != SDF_ARRAY_SET.end())
        return ARRAY;

    static std::set<std::string> already_printed;

    // check if the element name is in the map set
    if (SDF_MAP_SET.find(element_name) == SDF_MAP_SET.end() && already_printed.find(element_name) == already_printed.end())
    {
        already_printed.insert(element_name);
        std::cout << "Element: '" << element_name << "' not in SDF ARRAY or MAP list. Will return MAP as Node type." << std::endl;
    }

    // return MAP, even if it is not in the map set.
    return MAP;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromSDFElement(const TiXmlElement& element, ReaderWriter& config, const NodeType node_type)
{
    // Attributes aren't read, if element doesn't have any child elements
    if (element.FirstChildElement() == nullptr)
    {
        return loadFromXMLText(element, config);
    }
    else
    {
        // Start new group or array item
        // in case of array the array is opened on parent level. Only need to add array items on this level.
        if (node_type == ARRAY)
            config.addArrayItem();
        else
            // Start a new group
            config.writeGroup(element.ValueStr());


        // Iterate through attributes
        // if this element does not contain children, we don't end up here
        for (const TiXmlAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
        {
            setValue(attribute->NameTStr(), attribute->ValueStr(), config);
        }

        // Iterate through elements
        for(const TiXmlElement* e = element.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
        {
            std::string candidate_name = e->ValueStr();
            tue::config::NodeType candidate_node_type = getSDFNodeType(candidate_name);
            // Because potentially childs of same type could be read, alternated by an other type.
            // Hence opening and closing of the array is needed for each child.
            // wirteArray will open the array if existing.
            if (candidate_node_type == ARRAY && !config.writeArray(candidate_name))
            {
                std::stringstream error;
                error << "Could not write or read array: " << candidate_name;
                config.addError(error.str());
                return false;
            }

            // load child element
            if (!loadFromSDFElement(*e, config, candidate_node_type))
            {
                std::stringstream error;
                error << "Error parsing " << candidate_name;
                config.addError(error.str());
                return false;
            }

            // end array
            if (candidate_node_type == ARRAY)
                config.endArray();
        }
        // Ending array item or group
        if (node_type == ARRAY)
            config.endArrayItem();
        else
            config.endGroup();
    }

    return true;

}

// ----------------------------------------------------------------------------------------------------

bool loadFromSDFDocument(const TiXmlDocument& doc, ReaderWriter& config)
{
    const TiXmlElement* root = doc.FirstChildElement();

    if (root->NextSibling() != nullptr)
    {
        throw tue::config::ParseException("A valid XML file should only contain one root element");
    }

    return loadFromSDFElement(*root, config, MAP);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromSDFStream(std::istream& stream, ReaderWriter& config)
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
    return loadFromSDFDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromSDFString(const std::string& string, ReaderWriter& config)
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
    return loadFromSDFDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromSDFFile(const std::string& filename, ReaderWriter& config)
{
    config.setSource(filename);

    tue::filesystem::Path path(filename);
    if (!path.exists())
    {
        std::stringstream error;
        error << "[loadFromSDFFile] file '" << filename << "' doesn't exist." << std::endl;
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
    return loadFromSDFDocument(doc, config);
}

// ----------------------------------------------------------------------------------------------------

}  // End of namespace config

}  // End of namespace tue
