#include "tue/config/loaders/yaml.h"

#include "loader_functions.h"
#include "resolve_functions.h"
#include "tue/config/reader_writer.h"
#include "tue/config/resolve_config.h"

#include <sstream>
#include <string>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/detail/iterator_fwd.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/node/type.h>
// The forward-declaration headers above satisfy include-cleaner, but the complete YAML::Node
// iterator and node.as<T>() conversion definitions live behind the umbrella header.
#include <yaml-cpp/yaml.h> // IWYU pragma: keep

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

namespace
{

// NOLINTNEXTLINE(misc-no-recursion) — mutually recursive with loadFromYAMLNode through file includes.
bool yamlScalarToVariant(const std::string& key,
                         const YAML::Node& n,
                         ReaderWriter& config,
                         const ResolveConfig& resolve_config)
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
            std::filesystem::path const filepath(config.source());
            filename = (filepath.parent_path() / s).string();
        }

        return loadFromYAMLFile(filename, config, resolve_config);
    }

    // Check and resolve possible resolve functions ( "$( ... )" )
    std::string s_resolved;
    std::stringstream s_error;
    if (!resolve(s, config.source(), s_resolved, s_error, resolve_config))
    {
        config.addError("While reading key '" + key + "': Could not resolve: " + s_error.str());
        return false;
    }

    // Make sure that empty strings("") are set as string
    if (s_resolved.empty())
    {
        config.setValue(key, s_resolved);
        return true;
    }

    // strtol/strtod write the end pointer through &p_end, so its pointee cannot be const (the C API
    // signature is char**), even though we never modify the pointed-to characters ourselves.
    char* p_end = nullptr; // NOLINT(misc-const-correctness)
    const int i = static_cast<int>(std::strtol(s_resolved.c_str(), &p_end, 10));
    if (p_end[0] == 0)
    {
        config.setValue(key, i);
        return true;
    }

    const double d = std::strtod(s_resolved.c_str(), &p_end);
    if (p_end[0] == 0)
    {
        config.setValue(key, d);
        return true;
    }

    bool b = false;
    if (strToBool(s_resolved, b))
    {
        config.setValue(key, b);
        return true;
    }

    config.setValue(key, s_resolved);
    return true;
}

// ----------------------------------------------------------------------------------------------------

// NOLINTNEXTLINE(misc-no-recursion) — recursively walks nested maps and sequences.
bool loadFromYAMLNode(const YAML::Node& node, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    bool success = true;
    for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
    {
        auto const key = it->first.as<std::string>();
        // Copy the node by value: yaml-cpp's iterator returns a temporary
        // YAML::Node, so binding it to a reference would dangle. YAML::Node
        // has cheap shared-pointer semantics, so copying is safe.
        const YAML::Node n = it->second;

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
            // NOLINTNEXTLINE(modernize-loop-convert) — see the note above; index iteration is required.
            for (std::size_t i = 0; i < n.size(); ++i)
            {
                // Copy by value: YAML::Node::operator[] returns a temporary.
                const YAML::Node n2 = n[i];
                if (n2.Type() != YAML::NodeType::Map)
                {
                    config.addError("Sequences must only contains maps");
                    return false;
                }

                config.addArrayItem();
                loadFromYAMLNode(n2, config, resolve_config);
                config.endArrayItem();
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
        case YAML::NodeType::Null: break;
        }
    }

    return success;
}

} // namespace

// ----------------------------------------------------------------------------------------------------

// NOLINTNEXTLINE(misc-no-recursion) — part of the include-driven recursive load chain.
bool loadFromYAMLStream(std::istream& stream, ReaderWriter& config, const ResolveConfig& resolve_config)
{
    try
    {

        YAML::Node const doc = YAML::Load(stream);

        if (doc.Type() != YAML::NodeType::Map)
        {
            config.addError("Root of the config file must be a map.");
            return false;
        }

        if (!loadFromYAMLNode(doc, config, resolve_config))
            return false;
    }
    catch (YAML::Exception& e)
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

// NOLINTNEXTLINE(misc-no-recursion) — part of the include-driven recursive load chain.
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
    //    source_last_write_time_ = std::filesystem::last_write_time(filename_);

    return true;
}

} // namespace tue::config
