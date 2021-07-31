#ifndef TUE_CONFIG_TYPES_H_
#define TUE_CONFIG_TYPES_H_

#include <boost/shared_ptr.hpp>
#include "tue/config/data_pointer.h"

#include <string>

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

// Trim functions from https://stackoverflow.com/a/217605
// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {return !std::isspace(ch);}));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch);}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}

} // end namespace tue

#endif
