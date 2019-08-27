// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012019)

#ifndef DUNE_XT_COMMON_MKL_HH
#define DUNE_XT_COMMON_MKL_HH

namespace Dune {
namespace XT {
namespace Common {
namespace Mkl {


/**
 * \brief If true, calling any of the other methods makes sense.
 */
bool available();


/**
 * \brief Wrapper around MKL's vdExp
 * \sa    vdExp
 */
void exp(const int n, const double* a, double* y);


} // namespace Mkl
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_MKL_HH
