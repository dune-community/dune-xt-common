// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016 - 2017)
//   Rene Milk       (2015 - 2016)
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
  FieldMatrix(const K& kk = 0)
    : BaseType(kk)
  {
  }

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K& kk = 0)
    : BaseType(kk)
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
  } // ... FieldMatrix(...)

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType(0.)
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
}; // class FieldMatrix<...>

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
  FieldMatrix(const K& kk = 0)
    : BaseType(kk)
  {
  }

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType(0.)
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

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K& kk = 0)
    : BaseType(kk)
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
  } // ... FieldMatrix(...)

  FieldMatrix(const Dune::XT::Common::FieldVector<K, 1>& other)
    : BaseType(other[0])
  {
  }

  FieldMatrix(const Dune::FieldVector<K, 1>& other)
    : BaseType(other[0])
  {
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

  ThisType operator+(const ThisType& other) const
  {
    ThisType ret = *this;
    ret += other;
    return ret;
  }

  ThisType operator-(const ThisType& other) const
  {
    ThisType ret = *this;
    ret -= other;
    return ret;
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

  static const bool is_matrix = true;

  static const bool has_static_size = true;

  static const size_t static_rows = N;

  static const size_t static_cols = M;

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    return MatrixType(rows, cols);
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    return MatrixType(rows, cols, val);
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
imag(const Dune::FieldMatrix<K, ROWS, COLS>& real_mat)
{
  return FieldMatrix<K, ROWS, COLS>(0);
}

template <class K, int ROWS, int COLS>
typename std::enable_if<is_arithmetic<K>::value && !is_complex<K>::value, FieldMatrix<K, ROWS, COLS>>::type
imag(const FieldMatrix<K, ROWS, COLS>& real_mat)
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
operator-(const Dune::FieldMatrix<L, ROWS, COLS>& left, const Dune::FieldMatrix<R, COLS, COLS>& right)
{
  Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS> ret = left;
  ret -= right;
  return ret;
}


template <class L, int ROWS, int COLS, class R>
Dune::XT::Common::FieldMatrix<typename PromotionTraits<L, R>::PromotedType, ROWS, COLS>
operator+(const Dune::FieldMatrix<L, ROWS, COLS>& left, const Dune::FieldMatrix<R, COLS, COLS>& right)
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


} // namespace Dune

#endif // DUNE_XT_COMMON_FMATRIX_HH
