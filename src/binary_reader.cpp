#include "tue/config2/binary_reader.h"

#include <iostream>
#include <string.h>

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

BinaryReader::BinaryReader(const std::vector<char>& data)
    : data_(&data[0]), byte_size_(data.size()), idx_(0), error_(false)
{}

BinaryReader::BinaryReader(const char* data, unsigned int byte_size)
    : data_(data), byte_size_(byte_size), idx_(0), error_(false)
{}

// ----------------------------------------------------------------------------------------------------

bool BinaryReader::read(const std::string& name)
{
    char t = data_[idx_];
    if (t == 'G' || t == 'A')
    {
        ++idx_;
        if (strcmp(&data_[idx_], name.c_str()) == 0)
        {
            idx_ += name.size() + 1;
            return true;
        }
    }

    error_ = true;
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool BinaryReader::end()
{
    if (data_[idx_] == 'E')
    {
        ++idx_;
        return true;
    }

    error_ = true;
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool BinaryReader::next()
{
    if (data_[idx_] == 'I')
    {
        ++idx_;
        return true;
    }

    error_ = true;
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool BinaryReader::value(const std::string& name, int& value)
{
    if (data_[idx_] == 'V' && strcmp(&data_[++idx_], name.c_str()) == 0)
    {
        idx_ += name.size() + 1;
        memcpy((char*)&value, &data_[idx_], sizeof(value));
        idx_ += sizeof(value);

        return true;
    }

    error_ = true;
    return false;

}

} // end namespace tue

} // end namespace config

