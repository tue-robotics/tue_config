#include "tue/config/loaders/yaml.h"

#include "tue/config/configuration.h"
#include "resolve_functions.h"
#include "loader_functions.h"

#include <tue/filesystem/path.h>

// YAML parsing
#include <fstream>

#include "yaml-cpp/yaml.h"

#include <iostream>
#include <stdlib.h>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

bool yamlScalarToVariant(const std::string& key, const YAML::Node& n, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    std::string s;

    s = n.as<std::string>();

    if (key == "include")
    {
        std::string filename;
        if (s.substr(0) == "/")
            filename = s;
        else
        {
            tue::filesystem::Path filepath(config.source());
            filename = filepath.parentPath().join(s).string();
        }

        return loadFromYAMLFile(filename, config, resolve_config);
    }

    // Check and resolve possible resolve functions ( "$( ... )" )
    std::string s_resolved;
    std::stringstream s_error;
    if (!resolve(s, config.source(), s_resolved, s_error, resolve_config))
    {
        config.addError("While reading key '" + key +"': Could not resolve: " + s_error.str());
        return false;
    }

    // Make sure that empty strings("") are set as string
    if (s_resolved.empty())
    {
        config.setValue(key, s_resolved);
        return true;
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

    bool b;
    if (strToBool(s_resolved, b))
    {
        config.setValue(key, b);
        return true;
    }

    config.setValue(key, s_resolved);
    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLNode(const YAML::Node& node, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    bool success = true;
    for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        const YAML::Node& n = it->second;

        switch (n.Type())
        {
        case YAML::NodeType::Scalar:
        {
            if (!yamlScalarToVariant(key, n, config, resolve_config))
                success = false;
            break;
        }
        case YAML::NodeType::Sequence:
        {
            config.writeArray(key);

            /*
             * Need to use index iteration instead of iterator for sequences, because it goes behind the end.
             * See https://github.com/jbeder/yaml-cpp/issues/833
             */
            for(std::size_t i = 0; i < n.size(); ++i)
            {
                const YAML::Node& n2 = n[i];
                if (n2.Type() != YAML::NodeType::Map)
                {
                    config.addError("Sequences must only contains maps");
                    return false;
                }
                else
                {
                    config.addArrayItem();
                    loadFromYAMLNode(n2, config, resolve_config);
                    config.endArrayItem();
                }
            }

            config.endArray();

            break;
        }
        case YAML::NodeType::Map:
            config.writeGroup(key);
            loadFromYAMLNode(n, config, resolve_config);
            config.endGroup();
            break;
        case YAML::NodeType::Undefined:
        case YAML::NodeType::Null:
            break;
        }
    }

    return success;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLStream(std::istream& stream, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    try
    {

        YAML::Node doc = YAML::Load(stream);

        if (doc.Type() != YAML::NodeType::Map)
        {
            config.addError("Root of the config file must be a map.");
            return false;
        }

        if (!loadFromYAMLNode(doc, config, resolve_config))
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

bool loadFromYAMLString(const std::string& string, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    std::stringstream ss;
    ss << string;
    return loadFromYAMLStream(ss, config, resolve_config);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLFile(const std::string& filename, ReaderWriter& config, const ResolveConfig& resolve_config)
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

    if (!loadFromYAMLStream(fin, config, resolve_config))
        return false;

    // TODO:
//    filename_ = filename;
//    source_last_write_time_ = tue::filesystem::Path(filename_).lastWriteTime();

    return true;
}

}

}
