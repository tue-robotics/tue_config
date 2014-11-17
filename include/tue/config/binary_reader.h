#ifndef TUE_CONFIG2_BINARY_READER_H_
#define TUE_CONFIG2_BINARY_READER_H_

#include <string>
#include <vector>

namespace tue
{
namespace config
{

class BinaryReader
{

public:

    BinaryReader(const std::vector<char>& data);

    BinaryReader(const char* data, unsigned int byte_size);

    bool read(const std::string& name);

    bool end();

    bool next();

    bool value(const std::string& name, int& value);

private:

    const char* data_;

    unsigned int byte_size_;

    unsigned int idx_;

    bool error_;

};

} // end namespace tue

} // end namespace config

#endif
