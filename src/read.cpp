#include "tue/config/read.h"

#include "tue/config/loaders/yaml.h"
#include "tue/config/reader_writer.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

DataPointer fromString(const std::string& s, const ResolveConfig& resolve_config)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLString(s, reader, resolve_config))
        throw ParseException(reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

DataPointer fromStream(std::istream& s, const ResolveConfig& resolve_config)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLStream(s, reader, resolve_config))
        throw ParseException(reader.error());


    return reader.data();

}

// ----------------------------------------------------------------------------------------------------

DataPointer fromFile(const std::string& filename, const ResolveConfig& resolve_config)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLFile(filename, reader, resolve_config))
        throw ParseException("While parsing '" + filename + "':\n\n" + reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

DataPointer fromFile(const char* filename, const ResolveConfig& resolve_config)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLFile(filename, reader, resolve_config))
        throw ParseException("While parsing '" + std::string(filename) + "':\n\n" + reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

}
}
