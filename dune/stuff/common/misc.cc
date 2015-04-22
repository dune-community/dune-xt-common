#include <config.h>

#include "misc.hh"

#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <algorithm>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/print.hh>

extern char** environ;

namespace Dune {
namespace Stuff {
namespace Common {

void dump_environment(boost::filesystem::ofstream& file, std::string csv_sep)
{
  using namespace std;
  vector<string> header, values;
  for (char** current = environ; *current; current++) {
    string line(*current);
    const auto tokens = DSC::tokenize(line, "=");
    if (tokens.size() == 2) {
      header.push_back(tokens[0]);
      values.push_back(tokens[1]);
    }
  }
  std::copy(header.begin(), header.end(), PrefixOutputIterator<string>(file, csv_sep));
  file << '\n';
  std::copy(values.begin(), values.end(), PrefixOutputIterator<string>(file, csv_sep));
}

} // namespace Common
} // namepspace Stuff
} // namespace Dune
