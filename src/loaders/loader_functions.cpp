#include "loader_functions.h"

#include <algorithm>

namespace tue
{
namespace config
{

bool strToBool(std::string s, bool& rhs)
{
  static const struct {
    std::string truename, falsename;
  } names[] = {
      {"y", "n"}, {"yes", "no"}, {"true", "false"}, {"on", "off"},
  };

  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  for (unsigned i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
    if (names[i].truename == s) {
      rhs = true;
      return true;
    }

    if (names[i].falsename == s) {
      rhs = false;
      return true;
    }
  }

  return false;
}

}

}

