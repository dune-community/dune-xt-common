// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016 - 2017)
//   Rene Milk       (2015 - 2016, 2018)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_FMATRIX_HH
#define DUNE_XT_COMMON_FMATRIX_HH

#include <initializer_list>

#include <dune/common/fmatrix.hh>
#include <dune/common/fvector.hh>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/debug.hh>
#include <dune/xt/common/matrix.hh>
#include <dune/xt/common/fvector.hh>
#include <dune/xt/common/type_traits.hh>

namespace Dune {
namespace XT {
namespace Common {


/**
 * \todo We need to implement all operators from the base which return the base, to rather return ourselfes!
 */
template <class K, int ROWS, int COLS>
class FieldMatrix : public Dune::FieldMatrix<K, ROWS, COLS>
{
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  using typename BaseType::value_type;
  using typename BaseType::size_type;
  using typename BaseType::field_type;

  FieldMatrix(const K& kk = suitable_default<K>::value())
    : BaseType()
  {
    // This is required because BaseType(kk) does not work for std::string
    for (size_t i = 0; i < ROWS; ++i) {
      for (size_t j = 0; j < COLS; ++j)
        (*this)[i][j] = kk;
    }
  }

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr),
              const size_t DXTC_DEBUG_ONLY(cc),
              const K& kk = suitable_default<K>::value())
    : BaseType()
  {
#ifndef NDEBUG
    if (rr != ROWS || cc != COLS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                                    << "static size) with "
                                                                    << rr
                                                                    << " rows and "
                                                                    << cc
                                                                    << " columns!");
#endif // NDEBUG
    // This is required because BaseType(kk) does not work for std::string
    for (size_t i = 0; i < ROWS; ++i) {
      for (size_t j = 0; j < COLS; ++j)
        (*this)[i][j] = kk;
    }

  } // ... FieldMatrix(...)

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType()
  {
#ifndef NDEBUG
    if (list_of_rows.size() != ROWS)
      DUNE_THROW(
          Exceptions::wrong_input_given,
          "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                             << "static size) from a list modeling a matrix with "
                                                             << list_of_rows.size()
                                                             << " rows!");
#endif // NDEBUG
    size_t rr = 0;
    for (auto row : list_of_rows) {
#ifndef NDEBUG
      if (row.size() != COLS)
        DUNE_THROW(
            Exceptions::wrong_input_given,
            "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                               << "static size) from a list with a row of length "
                                                               << row.size()
                                                               << "!");
#endif // NDEBUG
      size_t cc = 0;
      for (auto entry : row)
        (*this)[rr][cc++] = entry;
      ++rr;
    }
  } // FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)

  FieldMatrix(const ThisType& other) = default;

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  Dune::XT::Common::FieldMatrix<K, COLS, ROWS> transpose() const
  {
    Dune::XT::Common::FieldMatrix<K, COLS, ROWS> ret;
    for (size_t rr = 0; rr < ROWS; ++rr)
      for (size_t cc = 0; cc < COLS; ++cc)
        ret[cc][rr] = (*this)[rr][cc];
    return ret;
  }

  Dune::XT::Common::FieldVector<K, ROWS> operator*(const Dune::FieldVector<K, COLS>& vec) const
  {
    Dune::FieldVector<K, ROWS> ret;
    this->mv(vec, ret);
    return ret;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  }

  template <class Func>
  void luDecomposition(ThisType& A, Func func) const;

  void invert();

private:
  // copy from dune/common/densematrix.hh, we have to copy it as it is a private member of Dune::DenseMatrix
  struct ElimPivot
  {
    ElimPivot(std::vector<size_type>& pivot)
      : pivot_(pivot)
    {
      typedef typename std::vector<size_type>::size_type size_type;
      for (size_type i = 0; i < pivot_.size(); ++i)
        pivot_[i] = i;
    }

    void swap(int i, int j)
    {
      pivot_[i] = j;
    }

    template <typename T>
    void operator()(const T&, int, int)
    {
    }

    std::vector<size_type>& pivot_;
  }; // struct ElimPivot
}; // class FieldMatrix<...>

// Direct copy of the luDecomposition function in dune/common/densematrix.hh
// The only (functional) change is that this version always performs pivotization (the version in dune-common only
// performs pivotization if the diagonal entry is below a certain threshold)
// See dune/xt/la/test/matrixinverter_for_real_matrix_from_3d_pointsource.tpl for an example where the dune-common
// version fails due to stability issues.
// TODO: Fixed in dune-common master (see MR !449 in dune-common's gitlab), remove this copy once we depend on a
// suitable version of dune-common (probably 2.7).
template <class K, int ROWS, int COLS>
template <typename Func>
inline void FieldMatrix<K, ROWS, COLS>::luDecomposition(FieldMatrix<K, ROWS, COLS>& A, Func func) const
{
  typedef typename FieldTraits<value_type>::real_type real_type;
  real_type norm = A.infinity_norm_real(); // for relative thresholds
  real_type singthres =
      std::max(FMatrixPrecision<real_type>::absolute_limit(), norm * FMatrixPrecision<real_type>::singular_limit());

  // LU decomposition of A in A
  for (size_type i = 0; i < ROWS; i++) // loop over all rows
  {
    typename FieldTraits<value_type>::real_type pivmax = fvmeta::absreal(A[i][i]);

    // compute maximum of column
    size_type imax = i;
    typename FieldTraits<value_type>::real_type abs(0.0);
    for (size_type k = i + 1; k < ROWS; k++)
      if ((abs = fvmeta::absreal(A[k][i])) > pivmax) {
        pivmax = abs;
        imax = k;
      }
    // swap rows
    if (imax != i) {
      for (size_type j = 0; j < ROWS; j++)
        std::swap(A[i][j], A[imax][j]);
      func.swap(i, imax); // swap the pivot or rhs
    }

    // singular ?
    if (pivmax < singthres)
      DUNE_THROW(FMatrixError, "matrix is singular");

    // eliminate
    for (size_type k = i + 1; k < ROWS; k++) {
      field_type factor = A[k][i] / A[i][i];
      A[k][i] = factor;
      for (size_type j = i + 1; j < ROWS; j++)
        A[k][j] -= factor * A[i][j];
      func(factor, k, i);
    }
  }
}

// Direct copy of the invert function in dune/common/densematrix.hh
// The only (functional) change is the replacement of the luDecomposition of DenseMatrix by our own version.
// TODO: Fixed in dune-common master (see MR !449 in dune-common's gitlab), remove this copy once we depend on a
// suitable version of dune-common (probably 2.7).
template <class K, int ROWS, int COLS>
inline void FieldMatrix<K, ROWS, COLS>::invert()
{
  // never mind those ifs, because they get optimized away
  if (ROWS != COLS)
    DUNE_THROW(Dune::FMatrixError, "Can't invert a " << ROWS << "x" << COLS << " matrix!");
  if (ROWS <= 3) {
    BaseType::invert();
  } else {
    auto A = *this;
    std::vector<size_type> pivot(ROWS);
    this->luDecomposition(A, ElimPivot(pivot));
    auto& L = A;
    auto& U = A;

    // initialize inverse
    *this = field_type();

    for (size_type i = 0; i < ROWS; ++i)
      (*this)[i][i] = 1;

    // L Y = I; multiple right hand sides
    for (size_type i = 0; i < ROWS; i++)
      for (size_type j = 0; j < i; j++)
        for (size_type k = 0; k < ROWS; k++)
          (*this)[i][k] -= L[i][j] * (*this)[j][k];

    // U A^{-1} = Y
    for (size_type i = ROWS; i > 0;) {
      --i;
      for (size_type k = 0; k < ROWS; k++) {
        for (size_type j = i + 1; j < ROWS; j++)
          (*this)[i][k] -= U[i][j] * (*this)[j][k];
        (*this)[i][k] /= U[i][i];
      }
    }

    for (size_type i = ROWS; i > 0;) {
      --i;
      if (i != pivot[i])
        for (size_type j = 0; j < ROWS; ++j)
          std::swap((*this)[j][pivot[i]], (*this)[j][i]);
    }
  }
}


/**
 * \todo We need to implement all operators from the base which return the base, to rather return ourselfes!
 */
template <class K>
class FieldMatrix<K, 1, 1> : public Dune::FieldMatrix<K, 1, 1>
{
  static const int ROWS = 1;
  static const int COLS = 1;
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  FieldMatrix(const K& kk = suitable_default<K>::value())
    : BaseType()
  {
    (*this)[0][0] = kk;
  }

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType()
  {
#ifndef NDEBUG
    if (list_of_rows.size() != ROWS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., 1, 1 > (of "
                     << "static size) from a list modeling a matrix with "
                     << list_of_rows.size()
                     << " rows!");
#endif // NDEBUG
    for (auto row : list_of_rows) {
#ifndef NDEBUG
      if (row.size() != COLS)
        DUNE_THROW(Exceptions::wrong_input_given,
                   "You are trying to construct a FieldMatrix< ..., 1, 1 > (of "
                       << "static size) from a list with a row of length "
                       << row.size()
                       << "!");
#endif // NDEBUG
      for (auto entry : row)
        (*this)[0][0] = entry;
    }
  } // FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr),
              const size_t DXTC_DEBUG_ONLY(cc),
              const K& kk = suitable_default<K>::value())
    : BaseType()
  {
#ifndef NDEBUG
    if (rr != ROWS || cc != COLS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                                    << "static size) with "
                                                                    << rr
                                                                    << " rows and "
                                                                    << cc
                                                                    << " columns!");
#endif // NDEBUG
    (*this)[0][0] = kk;
  } // ... FieldMatrix(...)

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  FieldMatrix(const Dune::XT::Common::FieldVector<K, 1>& other)
    : BaseType()
  {
    (*this)[0][0] = other[0];
  }

  FieldMatrix(const Dune::FieldVector<K, 1>& other)
    : BaseType()
  {
    (*this)[0][0] = other[0];
  }

  Dune::XT::Common::FieldMatrix<K, COLS, ROWS> transpose() const
  {
    return *this;
  }

  using BaseType::operator=;

  ThisType& operator=(const FieldVector<K, 1>& other)
  {
    BaseType::operator=(other[0]);
    return *this;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  }
}; // class FieldMatrix


template <class K, int N, int M>
struct MatrixAbstraction<Dune::XT::Common::FieldMatrix<K, N, M>>
{
  typedef Dune::XT::Common::FieldMatrix<K, N, M> MatrixType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;
  template <size_t rows = N, size_t cols = M, class FieldType = K>
  using MatrixTypeTemplate = Dune::XT::Common::FieldMatrix<FieldType, rows, cols>;

  static const bool is_matrix = true;

  static const bool has_static_size = true;

  static const size_t static_rows = N;

  static const size_t static_cols = M;

  static const constexpr StorageLayout storage_layout = StorageLayout::dense_row_major;

  template <size_t ROWS = static_rows, size_t COLS = static_cols, class SparsityPatternType = FullPattern>
  static inline MatrixTypeTemplate<ROWS, COLS> create(const size_t rows,
                                                      const size_t cols,
                                                      const ScalarType& val = suitable_default<ScalarType>::value(),
                                                      const SparsityPatternType& /*pattern*/ = SparsityPatternType())
  {
    return MatrixTypeTemplate<ROWS, COLS>(rows, cols, val);
  }

  static inline size_t rows(const MatrixType& /*mat*/)
  {
    return N;
  }

  static inline size_t cols(const MatrixType& /*mat*/)
  {
    return M;
  }

  static inline void set_entry(MatrixType& mat, const size_t row, const size_t col, const ScalarType& val)
  {
    mat[row][col] = val;
  }

  static inline ScalarType get_entry(const MatrixType& mat, const size_t row, const size_t col)
  {
    return mat[row][col];
  }

  static inline void add_to_entry(MatrixType& mat, const size_t row, const size_t col, const ScalarType& val)
  {
    mat[row][col] += val;
  }


  static inline ScalarType* data(MatrixType& mat)
  {
    return &(mat[0][0]);
  }

  static inline const ScalarType* data(const MatrixType& mat)
  {
    return &(mat[0][0]);
  }
};


template <class M>
typename std::enable_if<is_matrix<M>::value && MatrixAbstraction<M>::has_static_size,
                        std::unique_ptr<FieldMatrix<typename MatrixAbstraction<M>::S,
                                                    MatrixAbstraction<M>::static_rows,
                                                    MatrixAbstraction<M>::static_cols>>>::type
make_field_container_ptr(const M& mat)
{
  static const size_t rows = MatrixAbstraction<M>::static_rows;
  static const size_t cols = MatrixAbstraction<M>::static_cols;
  auto ret = std::make_unique<FieldMatrix<typename MatrixAbstraction<M>::S, rows, cols>>;
  for (size_t ii = 0; ii < rows; ++ii)
    for (size_t jj = 0; jj < cols; ++jj)
      (*ret)[ii][jj] = get_matrix_entry(mat, ii, jj);
  return std::move(ret);
}


template <class M>
typename std::enable_if<is_matrix<M>::value && MatrixAbstraction<M>::has_static_size,
                        FieldMatrix<typename MatrixAbstraction<M>::S,
                                    MatrixAbstraction<M>::static_rows,
                                    MatrixAbstraction<M>::static_cols>>::type
make_field_container(const M& mat)
{
  static const size_t rows = MatrixAbstraction<M>::static_rows;
  static const size_t cols = MatrixAbstraction<M>::static_cols;
  FieldMatrix<typename MatrixAbstraction<M>::S, rows, cols> ret;
  for (size_t ii = 0; ii < rows; ++ii)
    for (size_t jj = 0; jj < cols; ++jj)
      ret[ii][jj] = get_matrix_entry(mat, ii, jj);
  return ret;
}


template <class K, int ROWS, int COLS>
FieldMatrix<K, ROWS, COLS> make_field_container(Dune::FieldMatrix<K, ROWS, COLS>&& vec)
{
  return std::move(vec);
}


template <class K, int ROWS, int COLS>
FieldMatrix<K, ROWS, COLS> make_field_container(FieldMatrix<K, ROWS, COLS>&& vec)
{
  return std::move(vec);
}


template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
real(const Dune::FieldMatrix<K, ROWS, COLS>& real_mat)
{
  return real_mat;
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
real(const FieldMatrix<K, ROWS, COLS>& real_mat)
{
  return real_mat;
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
real(Dune::FieldMatrix<K, ROWS, COLS>&& real_mat)
{
  return std::move(real_mat);
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
real(FieldMatrix<K, ROWS, COLS>&& real_mat)
{
  return std::move(real_mat);
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_complex<K>::value, FieldMatrix<real_t<K>, ROWS, COLS>>::type
real(const Dune::FieldMatrix<K, ROWS, COLS>& complex_mat)
{
  FieldMatrix<real_t<K>, ROWS, COLS> real_mat;
  for (size_t ii = 0; ii < ROWS; ++ii)
    for (size_t jj = 0; jj < COLS; ++jj)
      real_mat[ii][jj] = complex_mat[ii][jj].real();
  return real_mat;
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_complex<K>::value, FieldMatrix<real_t<K>, ROWS, COLS>>::type
real(const FieldMatrix<K, ROWS, COLS>& complex_mat)
{
  FieldMatrix<real_t<K>, ROWS, COLS> real_mat;
  for (size_t ii = 0; ii < ROWS; ++ii)
    for (size_t jj = 0; jj < COLS; ++jj)
      real_mat[ii][jj] = complex_mat[ii][jj].real();
  return real_mat;
}


template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
imag(const Dune::FieldMatrix<K, ROWS, COLS>& /*real_mat*/)
{
  return FieldMatrix<K, ROWS, COLS>(0);
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
imag(const FieldMatrix<K, ROWS, COLS>& /*real_mat*/)
{
  return FieldMatrix<K, ROWS, COLS>(0);
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_complex<K>::value, FieldMatrix<real_t<K>, ROWS, COLS>>::type
imag(const Dune::FieldMatrix<K, ROWS, COLS>& complex_mat)
{
  FieldMatrix<real_t<K>, ROWS, COLS> real_mat;
  for (size_t ii = 0; ii < ROWS; ++ii)
    for (size_t jj = 0; jj < COLS; ++jj)
      real_mat[ii][jj] = complex_mat[ii][jj].imag();
  return real_mat;
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_complex<K>::value, FieldMatrix<real_t<K>, ROWS, COLS>>::type
imag(const FieldMatrix<K, ROWS, COLS>& complex_mat)
{
  FieldMatrix<real_t<K>, ROWS, COLS> real_mat;
  for (size_t ii = 0; ii < ROWS; ++ii)
    for (size_t jj = 0; jj < COLS; ++jj)
      real_mat[ii][jj] = complex_mat[ii][jj].imag();
  return real_mat;
}


} // namespace Common
} // namespace XT


template <class L, int ROWS, int COLS, class R>
Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS>
operator-(const Dune::FieldMatrix<L, ROWS, COLS>& left, const Dune::FieldMatrix<R, ROWS, COLS>& right)
{
  Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS> ret = left;
  ret -= right;
  return ret;
}


template <class L, int ROWS, int COLS, class R>
Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS>
operator+(const Dune::FieldMatrix<L, ROWS, COLS>& left, const Dune::FieldMatrix<R, ROWS, COLS>& right)
{
  Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS> ret = left;
  ret += right;
  return ret;
}


template <class K, int L_ROWS, int L_COLS, int R_COLS>
Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS> operator*(const Dune::FieldMatrix<K, L_ROWS, L_COLS>& left,
                                                           const Dune::FieldMatrix<K, L_COLS, R_COLS>& right)
{
  return left.rightmultiplyany(right);
}

template <class K, int L_ROWS, int L_COLS, int R_COLS>
void rightmultiply(Dune::FieldMatrix<K, L_ROWS, R_COLS>& ret,
                   const Dune::FieldMatrix<K, L_ROWS, L_COLS>& left,
                   const Dune::FieldMatrix<K, L_COLS, R_COLS>& right)
{
  for (size_t ii = 0; ii < L_ROWS; ++ii) {
    for (size_t jj = 0; jj < R_COLS; ++jj) {
      ret[ii][jj] = 0.;
      for (size_t kk = 0; kk < L_COLS; ++kk)
        ret[ii][jj] += left[ii][kk] * right[kk][jj];
    }
  }
}

template <class L, int L_ROWS, int L_COLS, class R, int R_COLS>
typename std::enable_if<!std::is_same<L, R>::value,
                        Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, L_ROWS, R_COLS>>::
    type
    operator*(const Dune::FieldMatrix<L, L_ROWS, L_COLS>& left, const Dune::FieldMatrix<R, L_COLS, R_COLS>& right)
{
  using Promoted = Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, L_ROWS, R_COLS>;
  using Dune::XT::Common::convert_to;
  return convert_to<Promoted>(left).rightmultiplyany(convert_to<Promoted>(right));
}

// versions that do not allocate matrices on the stack (for large matrices)
template <class K, int L_ROWS, int L_COLS, int R_COLS>
std::unique_ptr<Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS>>
operator*(const std::unique_ptr<Dune::FieldMatrix<K, L_ROWS, L_COLS>>& left,
          const Dune::FieldMatrix<K, L_COLS, R_COLS>& right)
{
  auto ret = std::make_unique<Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS>>();
  rightmultiply(*ret, *left, right);
  return ret;
}

template <class K, int L_ROWS, int L_COLS, int R_COLS>
std::unique_ptr<Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS>>
operator*(const Dune::FieldMatrix<K, L_ROWS, L_COLS>& left,
          const std::unique_ptr<Dune::FieldMatrix<K, L_COLS, R_COLS>>& right)
{
  auto ret = std::make_unique<Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS>>();
  rightmultiply(*ret, left, *right);
  return ret;
}

template <class K, int L_ROWS, int L_COLS, int R_COLS>
std::unique_ptr<Dune::XT::Common::FieldMatrix<K, L_ROWS, R_COLS>>
operator*(const std::unique_ptr<Dune::FieldMatrix<K, L_ROWS, L_COLS>>& left,
          const std::unique_ptr<Dune::FieldMatrix<K, L_COLS, R_COLS>>& right)
{
  return left * *right;
}


} // namespace Dune

#endif // DUNE_XT_COMMON_FMATRIX_HH
