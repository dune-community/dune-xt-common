// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2018)

#ifndef DUNE_XT_COMMON_BISECT_HH
#define DUNE_XT_COMMON_BISECT_HH

#include <functional>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/float_cmp.hh>

namespace Dune {
namespace XT {
namespace Common {


double find_largest_by_bisection(const double& left, const double& right, std::function<double(const double&)> test)
{
  double ll = (left < right) ? left : right;
  double rr = (left < right) ? right : left;
  if (test(rr))
    return rr;
  DUNE_THROW_IF(!test(ll), Exceptions::wrong_input_given, "");
  // no we know that ll is good, rr is bad
  while (FloatCmp::gt(rr, ll)) {
    const double middle = 0.5 * (ll + rr);
    if (test(middle))
      ll = middle;
    else
      rr = middle;
  }
  return rr;
} // ... find_largest_by_bisection(...)


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_BISECT_HH
