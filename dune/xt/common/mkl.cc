// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2019)

#include "config.h"

#if HAVE_MKL
#  include <mkl.h>
#else
#  include <cmath>
#endif

#include "mkl.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace Mkl {


bool available()
{
#if HAVE_MKL
  return true;
#else
  return false;
#endif
}


void exp(const int n, const double* a, double* y)
{
#if HAVE_MKL
  ::vdExp(n, a, y);
#else
  for (int ii = 0; ii < n; ++ii)
    y[ii] = std::exp(a[ii]);
#endif
}


} // namespace Mkl
} // namespace Common
} // namespace XT
} // namespace Dune
