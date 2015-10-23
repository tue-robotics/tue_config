#ifndef TUE_CONFIG_READ_H_
#define TUE_CONFIG_READ_H_

#include "tue/config/data_pointer.h"

namespace tue
{

namespace config
{

DataPointer fromString(const std::string& s);

DataPointer fromString(const char* s);

DataPointer fromStream(std::istream& s);

DataPointer fromFile(const std::string& filename);

DataPointer fromFile(const char* filename);

}
}

#endif
