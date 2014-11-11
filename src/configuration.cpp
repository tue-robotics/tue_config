#include "tue/config/configuration.h"
#include "tue/config/map.h"

#include <boost/make_shared.hpp>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

Configuration::Configuration()
{
    nodes.push_back(boost::make_shared<Map>(0));
}

// ----------------------------------------------------------------------------------------------------

Configuration::~Configuration()
{
}

}

} // end namespace tue

