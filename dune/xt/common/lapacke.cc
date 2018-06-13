// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017 - 2018)
//   Rene Milk       (2018)
//   Tobias Leibner  (2018)

#include "config.h"

#include <cmath>
// without the following lapacke will include <complex.h>, which will break dune/commontypetraits.hh^^
#include <complex>
#define lapack_complex_float std::complex<float>
#define lapack_complex_double std::complex<double>

#if HAVE_MKL
#include <mkl.h>
#else // HAVE_MKL
#if HAVE_LAPACKE
#include <lapacke.h>
#endif
#if HAVE_CBLAS
#include <cblas.h>
#endif
#endif // HAVE_MKL

#include <dune/common/unused.hh>

#include <dune/xt/common/exceptions.hh>

#include "lapacke.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace Mkl {


void exp(const int n, const double* a, double* y)
{
#if HAVE_MKL
  vdExp(n, a, y);
#else
  for (int ii = 0; ii < n; ++ii)
    y[ii] = std::exp(a[ii]);
#endif
}


} // namespace Mkl
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

int dgeev_work(int matrix_layout,
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
               int ldvr,
               double* work,
               int lwork)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeev_work(matrix_layout, jobvl, jobvr, n, a, lda, wr, wi, vl, ldvl, vr, ldvr, work, lwork);
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
  DUNE_UNUSED_PARAMETER(work);
  DUNE_UNUSED_PARAMETER(lwork);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dgeqp3(int matrix_layout, int m, int n, double* a, int lda, int* jpvt, double* tau)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeqp3(matrix_layout, m, n, a, lda, jpvt, tau);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(jpvt);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dgeqp3_work(int matrix_layout, int m, int n, double* a, int lda, int* jpvt, double* tau, double* work, int lwork)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeqp3_work(matrix_layout, m, n, a, lda, jpvt, tau, work, lwork);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(jpvt);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_UNUSED_PARAMETER(work);
  DUNE_UNUSED_PARAMETER(lwork);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dorgqr(int matrix_layout, int m, int n, int k, double* a, int lda, const double* tau)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dorgqr(matrix_layout, m, n, k, a, lda, tau);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dorgqr_work(int matrix_layout, int m, int n, int k, double* a, int lda, const double* tau, double* work, int lwork)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dorgqr_work(matrix_layout, m, n, k, a, lda, tau, work, lwork);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_UNUSED_PARAMETER(work);
  DUNE_UNUSED_PARAMETER(lwork);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dormqr(int matrix_layout,
           char side,
           char trans,
           int m,
           int n,
           int k,
           const double* a,
           int lda,
           const double* tau,
           double* c,
           int ldc)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dormqr(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(side);
  DUNE_UNUSED_PARAMETER(trans);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_UNUSED_PARAMETER(c);
  DUNE_UNUSED_PARAMETER(ldc);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dormqr_work(int matrix_layout,
                char side,
                char trans,
                int m,
                int n,
                int k,
                const double* a,
                int lda,
                const double* tau,
                double* c,
                int ldc,
                double* work,
                int lwork)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dormqr_work(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc, work, lwork);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(side);
  DUNE_UNUSED_PARAMETER(trans);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_UNUSED_PARAMETER(c);
  DUNE_UNUSED_PARAMETER(ldc);
  DUNE_UNUSED_PARAMETER(work);
  DUNE_UNUSED_PARAMETER(lwork);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dpotrf(int matrix_layout, char uplo, int n, double* a, int lda)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpotrf(matrix_layout, uplo, n, a, lda);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dpotrf_work(int matrix_layout, char uplo, int n, double* a, int lda)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpotrf_work(matrix_layout, uplo, n, a, lda);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
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


int zgeqp3(int matrix_layout, int m, int n, std::complex<double>* a, int lda, int* jpvt, std::complex<double>* tau)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zgeqp3(matrix_layout, m, n, a, lda, jpvt, tau);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(jpvt);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int zungqr(int matrix_layout, int m, int n, int k, std::complex<double>* a, int lda, const std::complex<double>* tau)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zungqr(matrix_layout, m, n, k, a, lda, tau);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int zunmqr(int matrix_layout,
           char side,
           char trans,
           int m,
           int n,
           int k,
           const std::complex<double>* a,
           int lda,
           const std::complex<double>* tau,
           std::complex<double>* c,
           int ldc)
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zunmqr(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc);
#else
  DUNE_UNUSED_PARAMETER(matrix_layout);
  DUNE_UNUSED_PARAMETER(side);
  DUNE_UNUSED_PARAMETER(trans);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(k);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(tau);
  DUNE_UNUSED_PARAMETER(c);
  DUNE_UNUSED_PARAMETER(ldc);
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
#if HAVE_MKL || HAVE_CBLAS
  return true;
#else
  return false;
#endif
}


int row_major()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasRowMajor;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int col_major()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasColMajor;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int left()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasLeft;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int right()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasRight;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int upper()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasUpper;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int lower()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasLower;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int trans()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasTrans;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int no_trans()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasNoTrans;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int unit()
{
#if HAVE_MKL || HAVE_CBLAS
  return CblasUnit;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
  return 1;
#endif
}


int non_unit()
{
#if HAVE_MKL || HAVE_CBLAS
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
#if HAVE_MKL || HAVE_CBLAS
  cblas_dtrsm(static_cast<CBLAS_LAYOUT>(layout),
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
#ifndef NDEBUG
  for (int ii = 0; ii < m; ++ii)
    if (std::isnan(b[ii]) || std::isinf(b[ii]))
      DUNE_THROW(Dune::MathError, "Triangular solve using cblas_dtrsm failed!");
#endif
#else
  DUNE_UNUSED_PARAMETER(layout);
  DUNE_UNUSED_PARAMETER(side);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(transa);
  DUNE_UNUSED_PARAMETER(diag);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(alpha);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(b);
  DUNE_UNUSED_PARAMETER(ldb);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void dtrsv(const int layout,
           const int uplo,
           const int transa,
           const int diag,
           const int n,
           const double* a,
           const int lda,
           double* x,
           const int incx)
{
#if HAVE_MKL || HAVE_CBLAS
  cblas_dtrsv(static_cast<CBLAS_LAYOUT>(layout),
              static_cast<CBLAS_UPLO>(uplo),
              static_cast<CBLAS_TRANSPOSE>(transa),
              static_cast<CBLAS_DIAG>(diag),
              n,
              a,
              lda,
              x,
              incx);
#ifndef NDEBUG
  for (int ii = 0; ii < n; ++ii)
    if (std::isnan(x[ii]) || std::isinf(x[ii]))
      DUNE_THROW(Dune::MathError, "Triangular solve using cblas_dtrsv failed!");
#endif
#else
  DUNE_UNUSED_PARAMETER(layout);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(transa);
  DUNE_UNUSED_PARAMETER(diag);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(x);
  DUNE_UNUSED_PARAMETER(incx);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void ztrsm(const int layout,
           const int side,
           const int uplo,
           const int transa,
           const int diag,
           const int m,
           const int n,
           const void* alpha,
           const void* a,
           const int lda,
           void* b,
           const int ldb)
{
#if HAVE_MKL || HAVE_CBLAS
  cblas_ztrsm(static_cast<CBLAS_LAYOUT>(layout),
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
#ifndef NDEBUG
  for (int ii = 0; ii < m; ++ii)
    if (std::isnan(std::abs(static_cast<std::complex<double>*>(b)[ii]))
        || std::isinf(std::abs(static_cast<std::complex<double>*>(b)[ii])))
      DUNE_THROW(Dune::MathError, "Triangular solve using cblas_ztrsm failed!");
#endif
#else
  DUNE_UNUSED_PARAMETER(layout);
  DUNE_UNUSED_PARAMETER(side);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(transa);
  DUNE_UNUSED_PARAMETER(diag);
  DUNE_UNUSED_PARAMETER(m);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(alpha);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(b);
  DUNE_UNUSED_PARAMETER(ldb);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void ztrsv(const int layout,
           const int uplo,
           const int transa,
           const int diag,
           const int n,
           const void* a,
           const int lda,
           void* x,
           const int incx)
{
#if HAVE_MKL || HAVE_CBLAS
  cblas_ztrsv(static_cast<CBLAS_LAYOUT>(layout),
              static_cast<CBLAS_UPLO>(uplo),
              static_cast<CBLAS_TRANSPOSE>(transa),
              static_cast<CBLAS_DIAG>(diag),
              n,
              a,
              lda,
              x,
              incx);
#ifndef NDEBUG
  for (int ii = 0; ii < n; ++ii)
    if (std::isnan(std::abs(static_cast<std::complex<double>*>(x)[ii]))
        || std::isinf(std::abs(static_cast<std::complex<double>*>(x)[ii])))
      DUNE_THROW(Dune::MathError, "Triangular solve using cblas_ztrsv failed!");
#endif
#else
  DUNE_UNUSED_PARAMETER(layout);
  DUNE_UNUSED_PARAMETER(uplo);
  DUNE_UNUSED_PARAMETER(transa);
  DUNE_UNUSED_PARAMETER(diag);
  DUNE_UNUSED_PARAMETER(n);
  DUNE_UNUSED_PARAMETER(a);
  DUNE_UNUSED_PARAMETER(lda);
  DUNE_UNUSED_PARAMETER(x);
  DUNE_UNUSED_PARAMETER(incx);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


} // namespace Blas
} // namespace Common
} // namespace XT
} // namespace Dune
