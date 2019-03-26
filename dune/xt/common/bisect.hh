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


/**
 * \brief Finds the largest number x between left and right, where condition(x) is true, but condition(y) is false for
 *        any y > x.
 *
 * \note Presumes that: if condition(x_1) == condition(x_2) == value,
 *                      then there does not exist a x_1 < y < x_2, s.t. condition(y) == !value;
 */
double find_largest_by_bisection(const double& left,
                                 const double& right,
                                 std::function<bool(const double&)> condition,
                                 const double rel_error = 1e-16,
                                 const double abs_error = 1e-16)
{
  double ll = (left < right) ? left : right;
  double rr = (left < right) ? right : left;
  if (condition(rr))
    return rr;
  DUNE_THROW_IF(!condition(ll), Exceptions::bisection_error, "");
  // no we know that ll is good, rr is bad
  while (FloatCmp::gt(rr, ll, rel_error, abs_error)) {
    const double middle = 0.5 * (ll + rr);
    if (condition(middle))
      ll = middle;
    else
      rr = middle;
  }
  return ll;
} // ... find_largest_by_bisection(...)


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_BISECT_HH
