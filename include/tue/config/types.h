#ifndef TUE_CONFIG2_TYPES_H_
#define TUE_CONFIG2_TYPES_H_

#include <boost/shared_ptr.hpp>
#include "tue/config/data_pointer.h"

namespace tue
{

namespace config
{

enum RequiredOrOptional
{
    REQUIRED,
    OPTIONAL
};

class Node;
typedef boost::shared_ptr<Node> NodePtr;
typedef boost::shared_ptr<const Node> NodeConstPtr;

}

} // end namespace tue

#endif
