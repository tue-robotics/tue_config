#include "tue/config/binary_writer.h"

#include <iostream>

namespace tue
{
namespace config
{

// ----------------------------------------------------------------------------------------------------

BinaryWriter::BinaryWriter()
{
}

// ----------------------------------------------------------------------------------------------------

BinaryWriter::~BinaryWriter()
{
}

// ----------------------------------------------------------------------------------------------------

bool BinaryWriter::writeGroup(const std::string& name) {
    data_.push_back('G');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
    return true; // To match api with non-binary writer
}

// ----------------------------------------------------------------------------------------------------

bool BinaryWriter::writeArray(const std::string& name) {
    data_.push_back('A');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
    return true; // To match api with non-binary writer
}

// ----------------------------------------------------------------------------------------------------

bool BinaryWriter::nextArrayItem() {
    data_.push_back('I');
    return true; // To match api with non-binary writer
}

// ----------------------------------------------------------------------------------------------------

bool BinaryWriter::end() {
    data_.push_back('E');
    return true; // To match api with non-binary writer
}

// ----------------------------------------------------------------------------------------------------

void BinaryWriter::setValue(const std::string& name, int value) {
    data_.push_back('V');
    data_.insert(data_.end(), name.begin(), name.end() + 1);
    data_.insert(data_.end(), (char*)&value, (char*)&value + sizeof(value));
}

void BinaryWriter::print() const
{
    std::cout << data_.size() << std::endl;
    for(unsigned int i = 0; i < data_.size(); ++i)
    {
        std::cout << (int)(data_[i]) << " ";
    }
    std::cout << std::endl;
}


} // end namespace tue

} // end namespace config

