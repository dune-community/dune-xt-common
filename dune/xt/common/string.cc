// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)

#include <config.h>

#include <algorithm>
#include <cctype>
#include <string>

#include "string.hh"

namespace Dune {
namespace XT {
namespace Common {


std::string to_lower(const std::string& ss)
{
  std::string out(ss);
  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}


} // namespace Common
} // namespace XT
} // namespace Dune
