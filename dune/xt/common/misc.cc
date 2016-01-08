// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2015)

#include <config.h>

#include "misc.hh"

#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <algorithm>
#include <dune/xt/common/string.hh>
#include <dune/xt/common/print.hh>

extern char** environ;

namespace Dune {
namespace XT {
namespace Common {

void dump_environment(boost::filesystem::ofstream& file, std::string csv_sep)
{
  using namespace std;
  vector<string> header, values;
  for (char** current = environ; *current; current++) {
    string line(*current);
    const auto tokens = tokenize(line, "=");
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
} // namespace XT
} // namespace Dune
