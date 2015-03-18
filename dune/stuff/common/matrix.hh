// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_COMMON_MATRIX_HH
#define DUNE_STUFF_COMMON_MATRIX_HH

#include <dune/common/dynmatrix.hh>
#include <dune/common/fmatrix.hh>

#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/fmatrix.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/common/type_utils.hh>

#if HAVE_DUNE_ISTL && HAVE_DUNE_FEM
// for dune-istl
#if defined(HAVE_BOOST)
#undef HAVE_BOOST
#define HAVE_BOOST 1
#endif
#include <dune/fem/operator/matrix/spmatrix.hh>
#include <dune/istl/operators.hh>
#include <dune/fem/operator/matrix/istlmatrix.hh>
#include <dune/fem/operator/matrix/preconditionerwrapper.hh>
#endif // HAVE_DUNE_ISTL && HAVE_DUNE_FEM

namespace Dune {
namespace Stuff {
namespace Common {


/**
 * \brief Traits to statically extract the scalar type of a (mathematical) vector-
 *
 *        If you want your vector class to benefit from the operators defined in this header you have to manually
 *        specify a specialization of this class in your code with is_matrix defined to true and an appropriate
 *        static create() method (see the specializations below).
 */
template <class MatType>
struct MatrixAbstraction
{
  typedef MatType MatrixType;
  typedef MatType ScalarType;
  typedef MatType S;

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

  static inline void set_entry(MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/,
                               const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*ScalarType*/ void get_entry(const MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }
};

template <class K>
struct MatrixAbstraction<Dune::DynamicMatrix<K>>
{
  typedef Dune::DynamicMatrix<K> MatrixType;
  typedef K ScalarType;
  typedef ScalarType S;

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
  typedef K ScalarType;
  typedef ScalarType S;

  static const bool is_matrix = true;

  static const bool has_static_size = true;

  static const size_t static_rows = N;

  static const size_t static_cols = M;

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    if (rows != N)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType();
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    if (rows != N)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType(val);
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

template <class K, int N, int M>
struct MatrixAbstraction<Dune::Stuff::Common::FieldMatrix<K, N, M>>
{
  typedef Dune::Stuff::Common::FieldMatrix<K, N, M> MatrixType;
  typedef K ScalarType;
  typedef ScalarType S;

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


template <class MatrixType>
struct is_matrix
{
  static const bool value = MatrixAbstraction<MatrixType>::is_matrix;
};


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, MatrixType>::type
create(const size_t sz, const typename MatrixAbstraction<MatrixType>::S& val)
{
  return MatrixAbstraction<MatrixType>::create(sz, val);
}

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MATRIX_HH
