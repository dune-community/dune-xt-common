// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)

#include "config.h"

// without the following lapacke will include <complex.h>, which will break dune/commontypetraits.hh^^
#include <complex>
#define lapack_complex_float std::complex<float>
#define lapack_complex_double std::complex<double>

#if HAVE_MKL
#include <mkl.h>
#elif HAVE_LAPACKE
#include <lapacke.h>
#endif

#include <dune/common/unused.hh>

#include "exceptions.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace Lapacke {


bool available()
{
#if HAVE_MKL || HAVE_LAPACKE
  return true;
#else
  return false;
#endif
}


int row_major()
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACK_ROW_MAJOR;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int col_major()
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACK_COL_MAJOR;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


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
          int ldvr)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeev(matrix_layout, jobvl, jobvr, n, a, lda, wr, wi, vl, ldvl, vr, ldvr);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(jobvl);
  DUNE_UNUSED_PARAMETER(jobvr);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(wr);
  DUNE_UNUSED_PARAMETER(wi);
  DUNE_UNUSED_PARAMETER(vl);
  DUNE_UNUSED_PARAMETER(ldvl);
  DUNE_UNUSED_PARAMETER(vr);
  DUNE_UNUSED_PARAMETER(ldvr);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dgeqp3(int matrix_layout, int m, int n, double* a, int lda, int* jpvt, double* tau)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeqp3(matrix_layout, m, n, a, lda, jpvt, tau);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dormqr(int matrix_layout,
           char side,
           char trans,
           lapack_int m,
           lapack_int n,
           lapack_int k,
           const double* a,
           lapack_int lda,
           const double* tau,
           double* c,
           lapack_int ldc)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dormqr(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dptcon(int n, const double* d, const double* e, double anorm, double* rcond)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dptcon(n, d, e, anorm, rcond);
#else
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(d);
  DUNE_UNUSED_PARAMETER(e);
  DUNE_UNUSED_PARAMETER(anorm);
  DUNE_UNUSED_PARAMETER(rcond);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dpttrf(int n, double* d, double* e)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpttrf(n, d, e);
#else
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(d);
  DUNE_UNUSED_PARAMETER(e);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dpttrs(int matrix_layout, int n, int nrhs, const double* d, const double* e, double* b, int ldb)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpttrs(matrix_layout, n, nrhs, d, e, b, ldb);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(nrhs);
  DUNE_UNUSED_PARAMETER(d);
  DUNE_UNUSED_PARAMETER(e);
  DUNE_UNUSED_PARAMETER(b);
  DUNE_UNUSED_PARAMETER(ldb);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


} // namespace Lapacke


namespace Blas {


/**
 * \brief If true, calling any of the other methods makes sense.
 */
bool available()
{
#if HAVE_MKL
  return true;
#else
  return false;
#endif
}


int row_major()
{
#if HAVE_MKL
  return CblasRowMajor;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int col_major()
{
#if HAVE_MKL
  return CblasColMajor;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int left()
{
#if HAVE_MKL
  return CblasLeft;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int right()
{
#if HAVE_MKL
  return CblasRight;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int upper()
{
#if HAVE_MKL
  return CblasUpper;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int lower()
{
#if HAVE_MKL
  return CblasLower;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int trans()
{
#if HAVE_MKL
  return CblasTrans;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int no_trans()
{
#if HAVE_MKL
  return CblasNoTrans;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int unit()
{
#if HAVE_MKL
  return CblasUnit;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int non_unit()
{
#if HAVE_MKL
  return CblasNonUnit;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


void dtrsm(const int layout,
           const int side,
           const int uplo,
           const int transa,
           const int diag,
           const int m,
           const int n,
           const double alpha,
           const double* a,
           const int lda,
           double* b,
           const int ldb)
{
#if HAVE_MKL
  return cblas_dtrsm(static_cast<CBLAS_LAYOUT>(layout),
                     static_cast<CBLAS_SIDE>(side),
                     static_cast<CBLAS_UPLO>(uplo),
                     static_cast<CBLAS_TRANSPOSE>(transa),
                     static_cast<CBLAS_DIAG>(diag),
                     m,
                     n,
                     alpha,
                     a,
                     lda,
                     b,
                     ldb);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


} // namespace Blas
} // namespace Common
} // namespace XT
} // namespace Dune
