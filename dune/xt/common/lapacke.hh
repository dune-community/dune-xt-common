// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016 - 2018)
//   Rene Milk       (2011 - 2012, 2015 - 2016, 2018)
//   Tobias Leibner  (2017 - 2018)

#ifndef DUNE_XT_COMMON_LAPACKE_HH
#define DUNE_XT_COMMON_LAPACKE_HH

#include <complex>

namespace Dune {
namespace XT {
namespace Common {
namespace Mkl {


/**
 * \brief Wrapper around MKL's vdExp
 * \sa    vdExp
 */
void exp(const int n, const double* a, double* y);


} // namespace Mkl
namespace Lapacke {


/**
 * \brief If true, calling any of the other methods makes sense.
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
 * \brief Wrapper around LAPACKE_dgeev_work
 * \sa    LAPACKE_dgeev_work
 */
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
               int lwork);


/**
 * \brief Wrapper around LAPACKE_dgeqp3
 * \sa    LAPACKE_dgeqp3
 */
int dgeqp3(int matrix_layout, int m, int n, double* a, int lda, int* jpvt, double* tau);

/**
 * \brief Wrapper around LAPACKE_dgeqp3_work
 * \sa    LAPACKE_dgeqp3
 */
int dgeqp3_work(int matrix_layout, int m, int n, double* a, int lda, int* jpvt, double* tau, double* work, int lwork);


/**
 * \brief Wrapper around LAPACKE_dorgqr
 * \sa    LAPACKE_dorgqr
 */
int dorgqr(int matrix_layout, int m, int n, int k, double* a, int lda, const double* tau);

/**
 * \brief Wrapper around LAPACKE_dorgqr_work
 * \sa    LAPACKE_dorgqr_work
 */
int dorgqr_work(int matrix_layout, int m, int n, int k, double* a, int lda, const double* tau, double* work, int lwork);


/**
 * \brief Wrapper around LAPACKE_dormqr
 * \sa    LAPACKE_dormqr
 */
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
           int ldc);

/**
 * \brief Wrapper around LAPACKE_dormqr_work
 * \sa    LAPACKE_dormqr_work
 */
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
                int lwork);


/**
 * \brief Wrapper around LAPACKE_dpotrf
 * \sa    LAPACKE_dpotrf
 */
int dpotrf(int matrix_layout, char uplo, int n, double* a, int lda);


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


/**
 * \brief Wrapper around LAPACKE_zgeqp3
 * \sa    LAPACKE_zgeqp3
 */
int zgeqp3(int matrix_layout, int m, int n, std::complex<double>* a, int lda, int* jpvt, std::complex<double>* tau);


/**
 * \brief Wrapper around LAPACKE_zunmqr
 * \sa    LAPACKE_zunmqr
 */
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
           int ldc);


/**
 * \brief Wrapper around LAPACKE_zungqr
 * \sa    LAPACKE_zungqr
 */
int zungqr(int matrix_layout, int m, int n, int k, std::complex<double>* a, int lda, const std::complex<double>* tau);


} // namespace Lapacke
namespace Blas {


/**
 * \brief If true, calling any of the other methods makes sense.
 */
bool available();


/**
 * \brief Wrapper around CBlasRowMajor
 * \sa    CblasRowMajor
 */
int row_major();


/**
 * \brief Wrapper around CBlasColMajor
 * \sa    CblasColMajor
 */
int col_major();


/**
 * \brief Wrapper around CBlasLeft
 * \sa    CblasLeft
 */
int left();


/**
 * \brief Wrapper around CBlasRight
 * \sa    CblasRight
 */
int right();


/**
 * \brief Wrapper around CblasUpper
 * \sa    CblasUpper
 */
int upper();


/**
 * \brief Wrapper around CblasLower
 * \sa    CblasLower
 */
int lower();


/**
 * \brief Wrapper around CblasTrans
 * \sa    CblasTrans
 */
int trans();


/**
 * \brief Wrapper around CblasNoTrans
 * \sa    CblasNoTrans
 */
int no_trans();


/**
 * \brief Wrapper around CblasUnit
 * \sa    CblasUnit
 */
int unit();


/**
 * \brief Wrapper around CblasNonUnit
 * \sa    CblasNonUnit
 */
int non_unit();


/**
 * \brief Wrapper around cblas_dtrsm
 * \sa    cblas_dtrsm
 */
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
           const int ldb);

/**
 * \brief Wrapper around cblas_dtrsv
 * \sa    cblas_dtrsv
 */
void dtrsv(const int layout,
           const int uplo,
           const int transa,
           const int diag,
           const int n,
           const double* a,
           const int lda,
           double* x,
           const int incx);

/**
 * \brief Wrapper around cblas_ztrsm
 * \sa    cblas_ztrsm
 */
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
           const int ldb);

/**
 * \brief Wrapper around cblas_ztrsv
 * \sa    cblas_ztrsv
 */
void ztrsv(const int layout,
           const int uplo,
           const int transa,
           const int diag,
           const int n,
           const void* a,
           const int lda,
           void* x,
           const int incx);


} // namespace Blas
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_LAPACKE_HH
