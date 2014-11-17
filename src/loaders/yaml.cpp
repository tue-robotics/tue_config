#include "tue/config/loaders/yaml.h"

#include "tue/config/configuration.h"

// YAML parsing
#include <fstream>
#include "yaml-cpp/parser.h"
#include "yaml-cpp/node.h"
#include <stdlib.h>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

Variant yamlScalarToVariant(const YAML::Node& n)
{
    std::string s;
    n.GetScalar(s);

    char* pEnd;

    int i = strtol(s.c_str(), &pEnd, 10);
    if (pEnd[0] == 0)
        return Variant(i);

    double d = strtod(s.c_str(), &pEnd);
    if (pEnd[0] == 0)
        return Variant(d);

    return Variant(s);
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLNode(const YAML::Node& node, ReaderWriter& config)
{

    for(YAML::Iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string key;
        it.first() >> key;

        const YAML::Node& n = it.second();

        switch (n.Type())
        {
        case YAML::NodeType::Scalar:
            config.setValue(key, yamlScalarToVariant(n));
            break;
        case YAML::NodeType::Sequence:
        {
            config.writeArray(key);

            for(YAML::Iterator it2 = n.begin(); it2 != n.end(); ++it2)
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
        // load yaml
        YAML::Parser parser(stream);
        YAML::Node doc;
        parser.GetNextDocument(doc);

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