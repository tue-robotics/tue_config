#ifndef TUE_CONFIG_WRITER_H_
#define TUE_CONFIG_WRITER_H_

#include <string>

namespace tue
{
namespace config2
{

class Writer
{

public:

    Writer() {}

    virtual ~Writer() {}

    virtual void writeBool(const std::string& key, bool v) {}

    virtual void writeInt(const std::string& key, int v) {}

    virtual void writeFloat(const std::string& key, double v) {}

    virtual void writeString(const std::string& key, const std::string& v) {}

    virtual void writeGroup(const std::string& name) {}

    virtual void endGroup() {}

    virtual void writeArray(const std::string& name) {}

    virtual void endArray() {}

    virtual void writeArrayItem() {}

    virtual void endArrayItem() {}

private:

};

} // end namespace tue

} // end namespace config2

#endif
