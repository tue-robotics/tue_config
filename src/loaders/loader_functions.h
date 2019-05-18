#ifndef TUE_CONFIG_LOADERS_LOADER_FUNCTIONS_H_
#define TUE_CONFIG_LOADERS_LOADER_FUNCTIONS_H_

#include <string>

namespace tue
{
namespace config
{

/**
 * @brief strToBool Converts a string to a boolean of matches one of the following: y, n, yes, no, true, false, on, off
 * @param s string to convert
 * @param rhs bool to set to true or false
 * @return indicates succes, rhs should only be used if return is true.
 */
bool strToBool(std::string s, bool& rhs);

}

}

#endif // TUE_CONFIG_LOADERS_LOADER_FUNCTIONS_H_
