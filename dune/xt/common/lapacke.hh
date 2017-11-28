// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Tobias Leibner (2017)

#ifndef DUNE_XT_COMMON_LAPACKE_HH
#define DUNE_XT_COMMON_LAPACKE_HH

namespace Dune {
namespace XT {
namespace Common {
namespace Lapacke {


/**
 * \brief If true, calling any of the other mathods makes sense.
 */
bool available();


/**
 * \brief Wrapper around LAPACK_ROW_MAJOR
 * \sa    LAPACK_ROW_MAJOR
 */
int row_major();


/**
 * \brief Wrapper around LAPACK_COL_MAJOR
 * \sa    LAPACK_COL_MAJOR
 */
int col_major();


/**
 * \brief Wrapper around LAPACKE_dgeev
 * \sa    LAPACKE_dgeev
 */
int dgeev(int matrix_layout,
          char jobvl,
          char jobvr,
          int n,
          double* a,
          int lda,
          double* wr,
          double* wi,
          double* vl,
          int ldvl,
          double* vr,
          int ldvr);


/**
 * \brief Wrapper around LAPACKE_dptcon
 * \sa    LAPACKE_dptcon
 */
int dptcon(int n, const double* d, const double* e, double anorm, double* rcond);


/**
 * \brief Wrapper around LAPACKE_dpttrf
 * \sa    LAPACKE_dpttrf
 */
int dpttrf(int n, double* d, double* e);


/**
 * \brief Wrapper around LAPACKE_dpttrs
 * \sa    LAPACKE_dpttrs
 */
int dpttrs(int matrix_layout, int n, int nrhs, const double* d, const double* e, double* b, int ldb);


} // namespace Lapacke
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_LAPACKE_HH
