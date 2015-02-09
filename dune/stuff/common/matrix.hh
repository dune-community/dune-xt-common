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

  static inline size_t rows(const MatrixType& /*mat*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline size_t cols(const MatrixType& /*mat*/)
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


/** \brief get the diagonal of a fieldMatrix into a fieldvector
   * \note While in principle this might do for SparseRowMatrix as well, don't do it! SparseRowMatrix has specialised
   *functions for this (putting the diagonal into DiscreteFunctions tho
   **/
template <class FieldMatrixType>
class MatrixDiagonal : public FieldVector<typename FieldMatrixType::field_type, FieldMatrixType::rows>
{
public:
  MatrixDiagonal(const FieldMatrixType& matrix)
  {
    static_assert(FieldMatrixType::rows == FieldMatrixType::cols, "Matrix must be square!");
    // CompileTimeChecker< FieldMatrixType::rows == FieldMatrixType::cols > matrix_must_be_square;

    for (auto i : valueRange(FieldMatrixType::rows))
      (*this)[i] = matrix[i][i];
  }
}; // class MatrixDiagonal

//! returns Sum of matrix' diagonal entries
template <class FieldMatrixType>
typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)
{
  MatrixDiagonal<FieldMatrixType> diag(matrix);
  typename FieldMatrixType::field_type trace = typename FieldMatrixType::field_type(0);
  for (auto i : valueRange(FieldMatrixType::rows))
    trace += diag[i];
  return trace;
} // typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)

//! produces a NxN Identity matrix compatible with parent type
template <class MatrixType>
class DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if "
                          "you need this (09.02.2015)!") IdentityMatrix : public MatrixType
{
public:
  IdentityMatrix(size_t N)
    : MatrixType(N, N, 1)
  {
    for (size_t i = 1; i < N; ++i)
      MatrixType::set(i, i, 1.0);
  }

  const MatrixType& matrix() const
  {
    return *this;
  }
}; // class IdentityMatrix


//! adds the missing setDiag function to SparseRowMatrix
template <class DiscFuncType, class MatrixType>
void setMatrixDiag(MatrixType& matrix, DiscFuncType& diag)
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix
  DofIteratorType it = diag.dbegin();

  for (auto row : valueRange(matrix.rows())) {
    if (*it != 0.0)
      matrix.set(row, row, *it);
    ++it;
  }
  return;
} // setMatrixDiag

//! return false if <pre>abs( a(row,col) - b(col,row) ) > tolerance<pre> for any col,row
template <class MatrixType>
bool areTransposed(const MatrixType& a, const MatrixType& b, const double tolerance = 1e-8)
{
  if ((a.rows() != b.cols()) || (b.rows() != a.cols()))
    return false;

  for (auto row : valueRange(a.rows())) {
    for (auto col : valueRange(a.cols())) {
      if (std::fabs(a(row, col) - b(col, row)) > tolerance)
        return false;
    }
  }
  return true;
} // areTransposed

//! extern matrix addition that ignore 0 entries
template <class MatrixType>
void addMatrix(MatrixType& dest, const MatrixType& arg, const double eps = 1e-14)
{
  for (auto i : valueRange(arg.rows()))
    for (auto j : valueRange(arg.cols())) {
      const double value = arg(i, j);
      if (std::fabs(value) > eps)
        dest.add(i, j, value);
    }

} // addMatrix

/** @brief Write sparse matrix to given output stream
   *
   *  @param[in]  matrix The matrix to be written
   *  @param[in] out    The outgoing stream
   */
template <class SparseMatrixImpl, class Output>
void writeSparseMatrix(const SparseMatrixImpl& matrix, Output& out)
{
  for (auto i : valueRange(matrix.rows())) {
    for (auto j : valueRange(matrix.cols())) {
      if (matrix.find(i, j)) {
        out << i << "," << j << ",";
        out.precision(12);
        out.setf(std::ios::scientific);
        out << matrix(i, j) << std::endl;
      }
    }
  }
  return;
} // writeSparseMatrix

/** @brief Read sparse matrix from given inputstream
   *
   *  @param[out] matrix The matrix to be read
   *  @param[in]  in     The ingoing stream
   */
template <class SparseMatrixImpl, class Input>
void readSparseMatrix(SparseMatrixImpl& matrix, Input& in)
{
  matrix.clear();
  std::string row;
  while (std::getline(in, row)) {
    size_t last_found = 0;
    size_t found      = row.find_first_of(",");
    double temp;
    std::vector<double> entryLine;
    do {
      if (found == std::string::npos)
        found          = row.size();
      std::string subs = row.substr(last_found, found - last_found);
      last_found = found + 1;
      std::istringstream in_str(subs);
      if (entryLine.size() == 2) {
        in_str.precision(12);
        in_str.setf(std::ios::scientific);
      } else {
        in_str.precision(10);
        in_str.setf(std::ios::fixed);
      }
      in_str >> temp;
      entryLine.push_back(temp);
      found = row.find_first_of(",", last_found);
    } while (last_found != row.size() + 1);
    assert(entryLine.size() == 3);
    matrix.add(entryLine[0], entryLine[1], entryLine[2]);
  }
} // readSparseMatrix

/**
   *  \brief  multiplies rows of arg2 with arg1
   *  \todo   doc
   **/
template <class FieldMatrixImp>
FieldMatrixImp rowWiseMatrixMultiplication(const FieldMatrixImp& arg1, const FieldMatrixImp& arg2)
{
  typedef FieldMatrixImp FieldMatrixType;
  typedef typename FieldMatrixType::row_type RowType;
  typedef typename FieldMatrixType::ConstRowIterator ConstRowIteratorType;
  typedef typename FieldMatrixType::RowIterator RowIteratorType;

  assert(arg2.rowdim() == arg1.coldim());

  FieldMatrixType ret(0.0);

  ConstRowIteratorType arg2RowItEnd = arg2.end();
  RowIteratorType retRowItEnd       = ret.end();
  RowIteratorType retRowIt = ret.begin();
  for (ConstRowIteratorType arg2RowIt = arg2.begin(); arg2RowIt != arg2RowItEnd, retRowIt != retRowItEnd;
       ++arg2RowIt, ++retRowIt) {
    RowType row(0.0);
    arg1.mv(*arg2RowIt, row);
    *retRowIt = row;
  }
  return ret;
} // rowWiseMatrixMultiplication

//! prints actual memusage of matrix in kB
template <class MatrixType>
void printMemUsage(const MatrixType& matrix, std::ostream& stream, std::string name = "")
{
  long size = matrix.numberOfValues() * sizeof(typename MatrixType::Ttype) / 1024.f;

  stream << "matrix size " << name << "\t\t" << size << std::endl;
}

//! prints actual memusage of matrixobject in kB
template <class MatrixObjectType>
void printMemUsageObject(const MatrixObjectType& matrix_object, std::ostream& stream, std::string name = "")
{
  printMemUsage(matrix_object.matrix(), stream, name);
}

template <class M>
void forceTranspose(const M& arg, M& dest)
{
  assert(arg.cols() == dest.rows());
  assert(dest.cols() == arg.rows());
  // dest.clear();
  for (auto i : valueRange(arg.cols()))
    for (auto j : valueRange(arg.rows()))
      dest.set(j, i, arg(i, j));
} // forceTranspose


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MATRIX_HH
