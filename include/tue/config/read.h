#ifndef TUE_CONFIG_READ_H_
#define TUE_CONFIG_READ_H_

#include "tue/config/data_pointer.h"
#include "tue/config/resolve_config.h"

#include <string>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

class ParseException : public std::exception
{
public:

    ParseException(const std::string message) : message_(message) {}

    ~ParseException() throw() {}

    const char* what() const throw() { return message_.c_str(); }

private:

    std::string message_;
};

// ----------------------------------------------------------------------------------------------------

DataPointer fromString(const std::string& s, const ResolveConfig& resolve_config=ResolveConfig());

DataPointer fromStream(std::istream& s, const ResolveConfig& resolve_config=ResolveConfig());

DataPointer fromFile(const std::string& filename, const ResolveConfig& resolve_config=ResolveConfig());

DataPointer fromFile(const char* filename, const ResolveConfig& resolve_config=ResolveConfig());

}
}

#endif
