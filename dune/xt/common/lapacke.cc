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

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/unused.hh>

#include "lapacke.hh"

#if HAVE_MKL || HAVE_LAPACKE
#define DXTC_LAPACKE_ONLY(param) param
#else
#define DXTC_LAPACKE_ONLY(param) DXTC_UNUSED(param)
#endif

#if HAVE_MKL || HAVE_CBLAS
#define DXTC_CBLAS_ONLY(param) param
#else
#define DXTC_CBLAS_ONLY(param) DXTC_UNUSED(param)
#endif

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


int dgeev(int DXTC_LAPACKE_ONLY(matrix_layout),
          char DXTC_LAPACKE_ONLY(jobvl),
          char DXTC_LAPACKE_ONLY(jobvr),
          int DXTC_LAPACKE_ONLY(n),
          double* DXTC_LAPACKE_ONLY(a),
          int DXTC_LAPACKE_ONLY(lda),
          double* DXTC_LAPACKE_ONLY(wr),
          double* DXTC_LAPACKE_ONLY(wi),
          double* DXTC_LAPACKE_ONLY(vl),
          int DXTC_LAPACKE_ONLY(ldvl),
          double* DXTC_LAPACKE_ONLY(vr),
          int DXTC_LAPACKE_ONLY(ldvr))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeev(matrix_layout, jobvl, jobvr, n, a, lda, wr, wi, vl, ldvl, vr, ldvr);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}
int dgeev_work(int DXTC_LAPACKE_ONLY(matrix_layout),
               char DXTC_LAPACKE_ONLY(jobvl),
               char DXTC_LAPACKE_ONLY(jobvr),
               int DXTC_LAPACKE_ONLY(n),
               double* DXTC_LAPACKE_ONLY(a),
               int DXTC_LAPACKE_ONLY(lda),
               double* DXTC_LAPACKE_ONLY(wr),
               double* DXTC_LAPACKE_ONLY(wi),
               double* DXTC_LAPACKE_ONLY(vl),
               int DXTC_LAPACKE_ONLY(ldvl),
               double* DXTC_LAPACKE_ONLY(vr),
               int DXTC_LAPACKE_ONLY(ldvr),
               double* DXTC_LAPACKE_ONLY(work),
               int DXTC_LAPACKE_ONLY(lwork))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeev_work(matrix_layout, jobvl, jobvr, n, a, lda, wr, wi, vl, ldvl, vr, ldvr, work, lwork);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dgeevx(int DXTC_LAPACKE_ONLY(matrix_layout),
           char DXTC_LAPACKE_ONLY(balanc),
           char DXTC_LAPACKE_ONLY(jobvl),
           char DXTC_LAPACKE_ONLY(jobvr),
           char DXTC_LAPACKE_ONLY(sense),
           int DXTC_LAPACKE_ONLY(n),
           double* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           double* DXTC_LAPACKE_ONLY(wr),
           double* DXTC_LAPACKE_ONLY(wi),
           double* DXTC_LAPACKE_ONLY(vl),
           int DXTC_LAPACKE_ONLY(ldvl),
           double* DXTC_LAPACKE_ONLY(vr),
           int DXTC_LAPACKE_ONLY(ldvr),
           int* DXTC_LAPACKE_ONLY(ilo),
           int* DXTC_LAPACKE_ONLY(ihi),
           double* DXTC_LAPACKE_ONLY(scale),
           double* DXTC_LAPACKE_ONLY(abnrm),
           double* DXTC_LAPACKE_ONLY(rconde),
           double* DXTC_LAPACKE_ONLY(rcondv))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeevx(matrix_layout,
                        balanc,
                        jobvl,
                        jobvr,
                        sense,
                        n,
                        a,
                        lda,
                        wr,
                        wi,
                        vl,
                        ldvl,
                        vr,
                        ldvr,
                        ilo,
                        ihi,
                        scale,
                        abnrm,
                        rconde,
                        rcondv);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dgeevx_work(int DXTC_LAPACKE_ONLY(matrix_layout),
                char DXTC_LAPACKE_ONLY(balanc),
                char DXTC_LAPACKE_ONLY(jobvl),
                char DXTC_LAPACKE_ONLY(jobvr),
                char DXTC_LAPACKE_ONLY(sense),
                int DXTC_LAPACKE_ONLY(n),
                double* DXTC_LAPACKE_ONLY(a),
                int DXTC_LAPACKE_ONLY(lda),
                double* DXTC_LAPACKE_ONLY(wr),
                double* DXTC_LAPACKE_ONLY(wi),
                double* DXTC_LAPACKE_ONLY(vl),
                int DXTC_LAPACKE_ONLY(ldvl),
                double* DXTC_LAPACKE_ONLY(vr),
                int DXTC_LAPACKE_ONLY(ldvr),
                int* DXTC_LAPACKE_ONLY(ilo),
                int* DXTC_LAPACKE_ONLY(ihi),
                double* DXTC_LAPACKE_ONLY(scale),
                double* DXTC_LAPACKE_ONLY(abnrm),
                double* DXTC_LAPACKE_ONLY(rconde),
                double* DXTC_LAPACKE_ONLY(rcondv),
                double* DXTC_LAPACKE_ONLY(work),
                int DXTC_LAPACKE_ONLY(lwork),
                int* DXTC_LAPACKE_ONLY(iwork))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeevx_work(matrix_layout,
                             balanc,
                             jobvl,
                             jobvr,
                             sense,
                             n,
                             a,
                             lda,
                             wr,
                             wi,
                             vl,
                             ldvl,
                             vr,
                             ldvr,
                             ilo,
                             ihi,
                             scale,
                             abnrm,
                             rconde,
                             rcondv,
                             work,
                             lwork,
                             iwork);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dgeqp3(int DXTC_LAPACKE_ONLY(matrix_layout),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           double* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           int* DXTC_LAPACKE_ONLY(jpvt),
           double* DXTC_LAPACKE_ONLY(tau))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeqp3(matrix_layout, m, n, a, lda, jpvt, tau);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dgeqp3_work(int DXTC_LAPACKE_ONLY(matrix_layout),
                int DXTC_LAPACKE_ONLY(m),
                int DXTC_LAPACKE_ONLY(n),
                double* DXTC_LAPACKE_ONLY(a),
                int DXTC_LAPACKE_ONLY(lda),
                int* DXTC_LAPACKE_ONLY(jpvt),
                double* DXTC_LAPACKE_ONLY(tau),
                double* DXTC_LAPACKE_ONLY(work),
                int DXTC_LAPACKE_ONLY(lwork))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dgeqp3_work(matrix_layout, m, n, a, lda, jpvt, tau, work, lwork);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dorgqr(int DXTC_LAPACKE_ONLY(matrix_layout),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           int DXTC_LAPACKE_ONLY(k),
           double* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           double* DXTC_LAPACKE_ONLY(tau))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dorgqr(matrix_layout, m, n, k, a, lda, tau);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dorgqr_work(int DXTC_LAPACKE_ONLY(matrix_layout),
                int DXTC_LAPACKE_ONLY(m),
                int DXTC_LAPACKE_ONLY(n),
                int DXTC_LAPACKE_ONLY(k),
                double* DXTC_LAPACKE_ONLY(a),
                int DXTC_LAPACKE_ONLY(lda),
                double* DXTC_LAPACKE_ONLY(tau),
                double* DXTC_LAPACKE_ONLY(work),
                int DXTC_LAPACKE_ONLY(lwork))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dorgqr_work(matrix_layout, m, n, k, a, lda, tau, work, lwork);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dormqr(int DXTC_LAPACKE_ONLY(matrix_layout),
           char DXTC_LAPACKE_ONLY(side),
           char DXTC_LAPACKE_ONLY(trans),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           int DXTC_LAPACKE_ONLY(k),
           const double* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           const double* DXTC_LAPACKE_ONLY(tau),
           double* DXTC_LAPACKE_ONLY(c),
           int DXTC_LAPACKE_ONLY(ldc))
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

int dormqr_work(int DXTC_LAPACKE_ONLY(matrix_layout),
                char DXTC_LAPACKE_ONLY(side),
                char DXTC_LAPACKE_ONLY(trans),
                int DXTC_LAPACKE_ONLY(m),
                int DXTC_LAPACKE_ONLY(n),
                int DXTC_LAPACKE_ONLY(k),
                const double* DXTC_LAPACKE_ONLY(a),
                int DXTC_LAPACKE_ONLY(lda),
                const double* DXTC_LAPACKE_ONLY(tau),
                double* DXTC_LAPACKE_ONLY(c),
                int DXTC_LAPACKE_ONLY(ldc),
                double* DXTC_LAPACKE_ONLY(work),
                int DXTC_LAPACKE_ONLY(lwork))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dormqr_work(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc, work, lwork);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dpotrf(int DXTC_LAPACKE_ONLY(matrix_layout),
           char DXTC_LAPACKE_ONLY(uplo),
           int DXTC_LAPACKE_ONLY(n),
           double* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpotrf(matrix_layout, uplo, n, a, lda);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}

int dpotrf_work(int DXTC_LAPACKE_ONLY(matrix_layout),
                char DXTC_LAPACKE_ONLY(uplo),
                int DXTC_LAPACKE_ONLY(n),
                double* DXTC_LAPACKE_ONLY(a),
                int DXTC_LAPACKE_ONLY(lda))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpotrf_work(matrix_layout, uplo, n, a, lda);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dptcon(int DXTC_LAPACKE_ONLY(n),
           const double* DXTC_LAPACKE_ONLY(d),
           const double* DXTC_LAPACKE_ONLY(e),
           double DXTC_LAPACKE_ONLY(anorm),
           double* DXTC_LAPACKE_ONLY(rcond))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dptcon(n, d, e, anorm, rcond);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int dpttrf(int DXTC_LAPACKE_ONLY(n), double* DXTC_LAPACKE_ONLY(d), double* DXTC_LAPACKE_ONLY(e))
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


int dpttrs(int DXTC_LAPACKE_ONLY(matrix_layout),
           int DXTC_LAPACKE_ONLY(n),
           int DXTC_LAPACKE_ONLY(nrhs),
           const double* DXTC_LAPACKE_ONLY(d),
           const double* DXTC_LAPACKE_ONLY(e),
           double* DXTC_LAPACKE_ONLY(b),
           int DXTC_LAPACKE_ONLY(ldb))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_dpttrs(matrix_layout, n, nrhs, d, e, b, ldb);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int zgeqp3(int DXTC_LAPACKE_ONLY(matrix_layout),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           std::complex<double>* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           int* DXTC_LAPACKE_ONLY(jpvt),
           std::complex<double>* DXTC_LAPACKE_ONLY(tau))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zgeqp3(matrix_layout, m, n, a, lda, jpvt, tau);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int zungqr(int DXTC_LAPACKE_ONLY(matrix_layout),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           int DXTC_LAPACKE_ONLY(k),
           std::complex<double>* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           const std::complex<double>* DXTC_LAPACKE_ONLY(tau))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zungqr(matrix_layout, m, n, k, a, lda, tau);
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lapacke or the intel mkl, check available() first!");
  return 1;
#endif
}


int zunmqr(int DXTC_LAPACKE_ONLY(matrix_layout),
           char DXTC_LAPACKE_ONLY(side),
           char DXTC_LAPACKE_ONLY(trans),
           int DXTC_LAPACKE_ONLY(m),
           int DXTC_LAPACKE_ONLY(n),
           int DXTC_LAPACKE_ONLY(k),
           const std::complex<double>* DXTC_LAPACKE_ONLY(a),
           int DXTC_LAPACKE_ONLY(lda),
           const std::complex<double>* DXTC_LAPACKE_ONLY(tau),
           std::complex<double>* DXTC_LAPACKE_ONLY(c),
           int DXTC_LAPACKE_ONLY(ldc))
{
#if HAVE_MKL || HAVE_LAPACKE
  return LAPACKE_zunmqr(matrix_layout, side, trans, m, n, k, a, lda, tau, c, ldc);
#else
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


void dtrsm(const int DXTC_CBLAS_ONLY(layout),
           const int DXTC_CBLAS_ONLY(side),
           const int DXTC_CBLAS_ONLY(uplo),
           const int DXTC_CBLAS_ONLY(transa),
           const int DXTC_CBLAS_ONLY(diag),
           const int DXTC_CBLAS_ONLY(m),
           const int DXTC_CBLAS_ONLY(n),
           const double DXTC_CBLAS_ONLY(alpha),
           const double* DXTC_CBLAS_ONLY(a),
           const int DXTC_CBLAS_ONLY(lda),
           double* DXTC_CBLAS_ONLY(b),
           const int DXTC_CBLAS_ONLY(ldb))
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
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void dtrsv(const int DXTC_CBLAS_ONLY(layout),
           const int DXTC_CBLAS_ONLY(uplo),
           const int DXTC_CBLAS_ONLY(transa),
           const int DXTC_CBLAS_ONLY(diag),
           const int DXTC_CBLAS_ONLY(n),
           const double* DXTC_CBLAS_ONLY(a),
           const int DXTC_CBLAS_ONLY(lda),
           double* DXTC_CBLAS_ONLY(x),
           const int DXTC_CBLAS_ONLY(incx))
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
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void ztrsm(const int DXTC_CBLAS_ONLY(layout),
           const int DXTC_CBLAS_ONLY(side),
           const int DXTC_CBLAS_ONLY(uplo),
           const int DXTC_CBLAS_ONLY(transa),
           const int DXTC_CBLAS_ONLY(diag),
           const int DXTC_CBLAS_ONLY(m),
           const int DXTC_CBLAS_ONLY(n),
           const void* DXTC_CBLAS_ONLY(alpha),
           const void* DXTC_CBLAS_ONLY(a),
           const int DXTC_CBLAS_ONLY(lda),
           void* DXTC_CBLAS_ONLY(b),
           const int DXTC_CBLAS_ONLY(ldb))
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
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


void ztrsv(const int DXTC_CBLAS_ONLY(layout),
           const int DXTC_CBLAS_ONLY(uplo),
           const int DXTC_CBLAS_ONLY(transa),
           const int DXTC_CBLAS_ONLY(diag),
           const int DXTC_CBLAS_ONLY(n),
           const void* DXTC_CBLAS_ONLY(a),
           const int DXTC_CBLAS_ONLY(lda),
           void* DXTC_CBLAS_ONLY(x),
           const int DXTC_CBLAS_ONLY(incx))
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
  DUNE_THROW(Exceptions::dependency_missing, "You are missing CBLAS or the intel mkl, check available() first!");
#endif
}


} // namespace Blas
} // namespace Common
} // namespace XT
} // namespace Dune
