#ifndef TUE_CONFIG_READER_H_
#define TUE_CONFIG_READER_H_

#include <string>

namespace tue
{
namespace config2
{

class Reader
{

public:

    Reader() {}

    virtual ~Reader() {}

    virtual bool readInt(const std::string& key, int& v) {}

    virtual bool readFloat(const std::string& key, double& v) {}

    virtual bool readString(const std::string& key, std::string& v) {}

    virtual bool readGroup(const std::string& name) {}

    virtual bool endGroup() {}

    virtual bool readArray(const std::string& name) {}

    virtual bool endArray() {}

private:

};

} // end namespace tue

} // end namespace config2

#endif
