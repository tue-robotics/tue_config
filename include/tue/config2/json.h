#ifndef TUE_CONFIG2_JSON_H_
#define TUE_CONFIG2_JSON_H_

#include <iostream>

namespace tue
{
namespace config2
{

struct Data;

namespace json
{

bool write(const Data& data, const std::string& filename);

bool write(const Data& data, std::ostream& out);

bool read(const std::string& filename, Data& data);

bool read(std::istream& in, Data& data);

}

} // end namespace tue

} // end namespace config2

#endif
