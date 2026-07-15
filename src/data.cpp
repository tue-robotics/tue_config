#include "tue/config/data.h"
#include "tue/config/map.h"

#include <boost/smart_ptr/make_shared_object.hpp>

#include "tue/config/yaml_emitter.h"

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

Data::Data()
{
    addNode(boost::make_shared<Map>(""), -1);
}

// ----------------------------------------------------------------------------------------------------

Data::~Data() = default;

// ----------------------------------------------------------------------------------------------------

// std::ostream& operator<<(std::ostream& out, const Data& d)
//{
//     YAMLEmitter emitter;
//     emitter.emit(DataPointer(d, d.root()), out);
//     return out;
// }

} // namespace tue::config
