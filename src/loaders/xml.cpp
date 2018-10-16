#include "tue/config/loaders/xml.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLStream(std::istream& stream, ReaderWriter& config){
  return false;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLString(const std::string& string, ReaderWriter& config)
{
  return false;
}

// ----------------------------------------------------------------------------------------------------

bool loadFromXMLFile(const std::string& filename, ReaderWriter& config)
{
  return false;
}

// ----------------------------------------------------------------------------------------------------

}  // End of namespace config

}  // End of namespace tue
