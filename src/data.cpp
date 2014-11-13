#include "tue/config/data.h"
#include "tue/config/map.h"

#include <boost/make_shared.hpp>

#include "tue/config/yaml_emitter.h"

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

Data::Data()
{
    addNode(boost::make_shared<Map>(""), -1);
}

// ----------------------------------------------------------------------------------------------------

Data::~Data()
{
}

// ----------------------------------------------------------------------------------------------------

//std::ostream& operator<<(std::ostream& out, const Data& d)
//{
//    YAMLEmitter emitter;
//    emitter.emit(DataPointer(d, d.root()), out);
//    return out;
//}

}

} // end namespace tue

