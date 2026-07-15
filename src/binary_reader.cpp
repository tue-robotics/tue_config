#include "tue/config/binary_reader.h"

#include <cstring>
#include <string>
#include <vector>

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

BinaryReader::BinaryReader(const std::vector<char>& data) :
    data_(data.data()), byte_size_(data.size()), idx_(0), error_(false)
{
}

BinaryReader::BinaryReader(const char* data, unsigned int byte_size) :
    data_(data), byte_size_(byte_size), idx_(0), error_(false)
{
}

// ----------------------------------------------------------------------------------------------------

bool BinaryReader::read(const std::string& name)
{
    char const t = data_[idx_];
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
    if (data_[idx_] == 'V')
    {
        ++idx_;
        if (strcmp(&data_[idx_], name.c_str()) == 0)
        {
            idx_ += name.size() + 1;
            memcpy(reinterpret_cast<char*>(&value), &data_[idx_], sizeof(value));
            idx_ += sizeof(value);

            return true;
        }
    }

    error_ = true;
    return false;
}

} // namespace tue::config
