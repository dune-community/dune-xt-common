// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2012 - 2017)
//   Rene Milk       (2010 - 2016)
//   Sven Kaulmann   (2010 - 2011)
//   Tobias Leibner  (2014, 2017)

#ifndef DUNE_XT_COMMON_MATRIX_HH
#define DUNE_XT_COMMON_MATRIX_HH

#include <memory>

#include <dune/common/dynmatrix.hh>
#include <dune/common/fmatrix.hh>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/vector.hh>

namespace Dune {
namespace XT {
namespace Common {


/**
 * \brief Traits to uniformly handle dense matrices.
 *
 *        If you want your matrix class to benefit from the functionality defined in this header you have to manually
 *        specify a specialization of this class in your code with is_matrix defined to true and the appropriate
 *        static methods implemented and members defined (see the specializations below).
 */
template <class MatType>
struct MatrixAbstraction
{
  typedef MatType MatrixType;
  typedef MatType ScalarType;
  typedef MatType RealType;
  typedef MatType S;
  typedef MatType R;

  static const bool is_matrix = false;

  static const bool has_static_size = false;

  static const size_t static_rows = std::numeric_limits<size_t>::max();

  static const size_t static_cols = std::numeric_limits<size_t>::max();

  static inline /*MatrixType*/ void create(const size_t /*rows*/, const size_t /*cols*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*MatrixType*/ void create(const size_t /*rows*/, const size_t /*cols*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*size_t*/ void rows(const MatrixType& /*mat*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*size_t*/ void cols(const MatrixType& /*mat*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline void
  set_entry(MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*ScalarType*/ void get_entry(const MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }
};


//! logically and structurally this belongs in type_utils.hh, but the dependent implementation prohibits that
template <class MatrixType>
struct is_matrix
{
  static const bool value = MatrixAbstraction<MatrixType>::is_matrix;
};


template <class K>
struct MatrixAbstraction<Dune::DynamicMatrix<K>>
{
  typedef Dune::DynamicMatrix<K> MatrixType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const bool is_matrix = true;

  static const bool has_static_size = false;

  static const size_t static_rows = std::numeric_limits<size_t>::max();

  static const size_t static_cols = std::numeric_limits<size_t>::max();

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    return MatrixType(rows, cols);
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    return MatrixType(rows, cols, val);
  }

  static inline size_t rows(const MatrixType& mat)
  {
    return mat.rows();
  }

  static inline size_t cols(const MatrixType& mat)
  {
    return mat.cols();
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

template <class K, int N, int M>
struct MatrixAbstraction<Dune::FieldMatrix<K, N, M>>
{
  typedef Dune::FieldMatrix<K, N, M> MatrixType;
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
    if (rows != N)
      DUNE_THROW(Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType();
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    if (rows != N)
      DUNE_THROW(Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType(val);
  }

  static inline size_t rows(const MatrixType& /*mat*/)
  {
    return numeric_cast<size_t>(N);
  }

  static inline size_t cols(const MatrixType& /*mat*/)
  {
    return numeric_cast<size_t>(M);
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


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, size_t>::type get_matrix_rows(const MatrixType& matrix)
{
  return MatrixAbstraction<MatrixType>::rows(matrix);
}


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, size_t>::type get_matrix_cols(const MatrixType& matrix)
{
  return MatrixAbstraction<MatrixType>::cols(matrix);
}


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, typename MatrixAbstraction<MatrixType>::S>::type
get_matrix_entry(const MatrixType& matrix, const size_t ii, const size_t jj)
{
  return MatrixAbstraction<MatrixType>::get_entry(matrix, ii, jj);
}


template <class MatrixType, class S>
typename std::enable_if<is_matrix<MatrixType>::value, void>::type
set_matrix_entry(MatrixType& matrix, const size_t ii, const size_t jj, const S& value)
{
  MatrixAbstraction<MatrixType>::set_entry(matrix, ii, jj, value);
}


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, MatrixType>::type
create(const size_t rows, const size_t cols, const typename MatrixAbstraction<MatrixType>::S& val = 0)
{
  return MatrixAbstraction<MatrixType>::create(rows, cols, val);
}


template <class T, class M>
typename std::enable_if<is_matrix<M>::value && is_arithmetic<T>::value, std::unique_ptr<T[]>>::type
serialize_rowwise(const M& mat)
{
  using Mat = MatrixAbstraction<M>;
  const size_t rows = Mat::rows(mat);
  const size_t cols = Mat::cols(mat);
  auto data = std::make_unique<T[]>(rows * cols);
  size_t ii = 0;
  for (size_t rr = 0; rr < rows; ++rr)
    for (size_t cc = 0; cc < cols; ++cc)
      data[ii++] = numeric_cast<T>(Mat::get_entry(mat, rr, cc));
  return data;
} // ... serialize_rowwise(...)


template <class M>
typename std::enable_if<is_matrix<M>::value, std::unique_ptr<typename MatrixAbstraction<M>::S[]>>::type
serialize_rowwise(const M& mat)
{
  return serialize_rowwise<typename MatrixAbstraction<M>::ScalarType>(mat);
}


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_MATRIX_HH
