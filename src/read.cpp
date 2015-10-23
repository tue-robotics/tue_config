#include "tue/config/read.h"

#include "tue/config/loaders/yaml.h"
#include "tue/config/reader_writer.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

DataPointer fromString(const std::string& s)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLString(s, reader))
        throw ParseException(reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

DataPointer fromStream(std::istream& s)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLStream(s, reader))
        throw ParseException(reader.error());


    return reader.data();

}

// ----------------------------------------------------------------------------------------------------

DataPointer fromFile(const std::string& filename)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLFile(filename, reader))
        throw ParseException("While parsing '" + filename + "':\n\n" + reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

DataPointer fromFile(const char* filename)
{
    ReaderWriter reader;
    if (!tue::config::loadFromYAMLFile(filename, reader))
        throw ParseException("While parsing '" + std::string(filename) + "':\n\n" + reader.error());

    return reader.data();
}

// ----------------------------------------------------------------------------------------------------

}
}
