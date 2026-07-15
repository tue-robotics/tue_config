#include "loader_functions.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace tue::config
{

bool strToBool(std::string s, bool& rhs)
{
    static const struct
    {
        std::string truename, falsename;
    } names[] = {
        {"y", "n"},
        {"yes", "no"},
        {"true", "false"},
        {"on", "off"},
    };

    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    for (const auto& name : names)
    {
        if (name.truename == s)
        {
            rhs = true;
            return true;
        }

        if (name.falsename == s)
        {
            rhs = false;
            return true;
        }
    }

    return false;
}

} // namespace tue::config
