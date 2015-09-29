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


    virtual bool readInt(const std::string& key, int& v) { return false; }

    virtual bool readFloat(const std::string& key, double& v) { return false; }

    virtual bool readString(const std::string& key, std::string& v) { return false; }


    virtual bool readGroup(const std::string& name) { return false; }

    virtual bool endGroup() { return false; }


    virtual bool readArray(const std::string& name) { return false; }

    virtual bool endArray() { return false; }

    virtual bool nextArrayItem() { return false; }


    virtual void addError(const std::string& msg) { }

    virtual void setErrorContext(const std::string& context) {}

    virtual bool hasError() const { return false; }

    virtual std::string error() const { return std::string(); }


private:

};

} // end namespace tue

} // end namespace config2

#endif
