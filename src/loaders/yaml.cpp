#include "tue/config/loaders/yaml.h"

#include "tue/config/configuration.h"
#include "resolve_functions.h"

// YAML parsing
#include <fstream>

#ifdef YAML_VERSION_0_3
    #include "yaml-cpp/parser.h"
    #include "yaml-cpp/node.h"
#else
    #include "yaml-cpp/yaml.h"
#endif

#include <iostream>
#include <stdlib.h>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

bool yamlScalarToVariant(const std::string& key, const YAML::Node& n, ReaderWriter& config)
{
    std::string s;

#ifdef YAML_VERSION_0_3
    n.GetScalar(s);

    // Check and resolve possible resolve functions ( "$( ... )" )
    std::string s_resolved;
    std::stringstream s_error;
    if (!resolve(s, config.source(), s_resolved, s_error))
    {
        config.addError("While reading key '" + key +"': Could not resolve: " + s_error.str());
        return false;
    }

    char* pEnd;

    int i = strtol(s_resolved.c_str(), &pEnd, 10);
    if (pEnd[0] == 0)
    {
        config.setValue(key, i);
        return true;
    }

    double d = strtod(s_resolved.c_str(), &pEnd);
    if (pEnd[0] == 0)
    {
        config.setValue(key, d);
        return true;
    }

    config.setValue(key, s_resolved);
    return true;
#else

    try
    {
        config.setValue(key, n.as<int>());
        return true;
    }
    catch (YAML::BadConversion& e) {}

    try
    {
        config.setValue(key, n.as<double>());
        return true;
    }
    catch (YAML::BadConversion& e) {}

    s = n.as<std::string>();

    // Check and resolve possible resolve functions ( "$( ... )" )
    std::string s_resolved;
    std::stringstream s_error;
    if (!resolve(s, config.source(), s_resolved, s_error))
    {
        config.addError("While reading key '" + key +"': Could not resolve: " + s_error.str());
        return false;
    }

    char* pEnd;
    int i = (int) std::strtol(s_resolved.c_str(), &pEnd, 10);
    if (pEnd[0] == 0)
    {
        config.setValue(key, i);
        return true;
    }

    double d = std::strtod(s_resolved.c_str(), &pEnd);
    if (pEnd[0] == 0)
    {
        config.setValue(key, d);
        return true;
    }

    config.setValue(key, s_resolved);
    return true;

#endif
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLNode(const YAML::Node& node, ReaderWriter& config)
{

#ifdef YAML_VERSION_0_3
    for(YAML::Iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string key;
        it.first() >> key;
        const YAML::Node& n = it.second();
#else
    for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        const YAML::Node& n = it->second;
#endif

        switch (n.Type())
        {
        case YAML::NodeType::Scalar:
        {
            yamlScalarToVariant(key, n, config);
            break;
        }
        case YAML::NodeType::Sequence:
        {
            config.writeArray(key);

#ifdef YAML_VERSION_0_3
            for(YAML::Iterator it2 = n.begin(); it2 != n.end(); ++it2)
#else
            for(YAML::const_iterator it2 = n.begin(); it2 != n.end(); ++it2)
#endif
            {
                const YAML::Node& n2 = *it2;
                if (n2.Type() != YAML::NodeType::Map)
                {
                    config.addError("Sequences must only contains maps");
                    return false;
                }
                else
                {
                    config.addArrayItem();
                    loadFromYAMLNode(n2, config);
                    config.endArrayItem();
                }
            }

            config.endArray();

            break;
        }
        case YAML::NodeType::Map:
            config.writeGroup(key);
            loadFromYAMLNode(n, config);
            config.endGroup();
            break;
        case YAML::NodeType::Null:
            break;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLStream(std::istream& stream, ReaderWriter& config)
{
    try
    {

#ifdef YAML_VERSION_0_3
        // load yaml
        YAML::Parser parser(stream);
        YAML::Node doc;
        parser.GetNextDocument(doc);
#else
        YAML::Node doc = YAML::Load(stream);
#endif

        if (doc.Type() != YAML::NodeType::Map)
        {
            config.addError("Root of the config file must be a map.");
            return false;
        }

        if (!loadFromYAMLNode(doc, config))
            return false;
    }
    catch(YAML::Exception& e)
    {
        config.addError(e.what());
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLString(const std::string& string, ReaderWriter& config)
{
    std::stringstream ss;
    ss << string;
    return loadFromYAMLStream(ss, config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLFile(const std::string& filename, ReaderWriter& config)
{
    // Remove possible previous errors (TODO)
//    config.data_->error.clear();

    // Reset head (TODO)
//    config.head_ = scope_;

    config.setSource(filename);

    std::ifstream fin(filename.c_str());
    if (fin.fail())
    {
        config.addError("No such file: '" + filename + "'.");
        return false;
    }

    if (!loadFromYAMLStream(fin, config))
        return false;

    // TODO:
//    filename_ = filename;
//    source_last_write_time_ = tue::filesystem::Path(filename_).lastWriteTime();

    return true;
}

}

}
