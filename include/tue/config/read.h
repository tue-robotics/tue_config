#ifndef TUE_CONFIG_READ_H_
#define TUE_CONFIG_READ_H_

#include "tue/config/data_pointer.h"
#include "tue/config/error.h"

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

DataPointer fromString(const std::string& s);

DataPointer fromStream(std::istream& s);

DataPointer fromFile(const std::string& filename);

DataPointer fromFile(const char* filename);

}
}

#endif
