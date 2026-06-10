#include "tue/config/binary_writer.h"

#include <console_bridge/console.h>

#include <sstream>

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

BinaryWriter::BinaryWriter() {}

// ----------------------------------------------------------------------------------------------------

BinaryWriter::~BinaryWriter() {}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::writeGroup(const std::string& name)
{
    data_.push_back('G');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::writeArray(const std::string& name)
{
    data_.push_back('A');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::nextArrayItem()
{
    data_.push_back('I');
}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::end()
{
    data_.push_back('E');
}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::setValue(const std::string& name, int value)
{
    data_.push_back('V');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
    data_.insert(data_.end(), (char*)&value, (char*)&value + sizeof(value));
}

void BinaryWriter::print() const
{
    std::stringstream ss;
    for (unsigned int i = 0; i < data_.size(); ++i)
    {
        ss << (int)(data_[i]) << " ";
    }
    CONSOLE_BRIDGE_logDebug("BinaryWriter size: %zu", data_.size());
    CONSOLE_BRIDGE_logDebug("%s", ss.str().c_str());
}

} // namespace config

} // namespace tue
