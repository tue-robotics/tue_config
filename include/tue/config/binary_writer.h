#ifndef TUE_CONFIG_BINARY_WRITER_H_
#define TUE_CONFIG_BINARY_WRITER_H_

#include <string>
#include <vector>

namespace tue
{
namespace config
{

class BinaryWriter
{

public:

    BinaryWriter();

    virtual ~BinaryWriter();

    void writeGroup(const std::string& name);

    void writeArray(const std::string& name);

    void nextArrayItem();

    void end();

    void setValue(const std::string& name, int value);

    unsigned int byte_size() const { return data_.size(); }

    void print() const;

    const char* dataPointer() const { return &data_[0]; }

private:

    std::vector<char> data_;

};

} // end namespace tue

} // end namespace config

#endif
