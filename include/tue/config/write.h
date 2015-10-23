#ifndef TUE_CONFIG_WRITE_H_
#define TUE_CONFIG_WRITE_H_

#include "tue/config/data_pointer.h"

namespace tue
{

namespace config
{

enum WriteType
{
    YAML,
    JSON
};

void toStream(std::ostream& s, const DataConstPointer& data, WriteType write_type, int indent_size = 0);

bool toFile(const std::string& filename, const DataConstPointer& data, WriteType write_type, int indent_size = 0);

bool toFile(const char* filename, const DataConstPointer& data, WriteType write_type, int indent_size = 0);

}
}

#endif
