#include "misc.hh"

#include <stdlib.h>
#include <stdio.h>

extern char** environ;

namespace Dune {
namespace Stuff {
namespace Common {

void dump_environment(boost::filesystem::ofstream& file)
{
  for (char** current = environ; *current; current++) {
    file << *current << '\n';
  }
}

} // namespace Common
} // namepspace Stuff
} // namespace Dune
