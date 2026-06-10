#ifndef TUE_CONFIG_LOADERS_RESOLVE_FUNCTIONS_H_
#define TUE_CONFIG_LOADERS_RESOLVE_FUNCTIONS_H_

#include "tue/config/resolve_config.h"

#include <sstream>
#include <string>

namespace tue
{

namespace config
{

bool resolve(const std::string& str,
             const std::string& source,
             std::string& result,
             std::stringstream& error,
             const ResolveConfig& config = ResolveConfig::defaultConfig());

}

} // namespace tue

#endif
