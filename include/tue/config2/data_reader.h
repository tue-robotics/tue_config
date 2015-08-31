#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include "tue/config2/reader.h"

namespace tue
{
namespace config2
{

class DataReader : public Reader
{

public:

    DataReader() {}

    ~DataReader() {}

    bool readInt(const std::string& key, int& v) { return false; }

    bool readFloat(const std::string& key, double& v) { return false; }

    bool readString(const std::string& key, std::string& v) { return false; }

    bool readGroup(const std::string& name) { return false; }

    bool endGroup() { return false; }

    bool readArray(const std::string& name) { return false; }

    bool endArray() { return false; }

private:

};

} // end namespace tue

} // end namespace config2

#endif
