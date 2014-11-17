#ifndef TUE_CONFIG2_BINARY_WRITER_H_
#define TUE_CONFIG2_BINARY_WRITER_H_

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

    bool writeGroup(const std::string& name);

    bool writeArray(const std::string& name);

    bool nextArrayItem();

    bool end();

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
