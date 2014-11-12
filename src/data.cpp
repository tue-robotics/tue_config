#include "tue/config/data.h"
#include "tue/config/map.h"

#include <boost/make_shared.hpp>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

Configuration::Configuration()
{
    addNode(boost::make_shared<Map>(0), -1);
}

// ----------------------------------------------------------------------------------------------------

Configuration::~Configuration()
{
}

}

} // end namespace tue

