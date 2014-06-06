// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_COMMON_MATRIX_HH
#define DUNE_STUFF_COMMON_MATRIX_HH

// dune-common
#include <dune/common/densematrix.hh>

#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/math.hh>

#if HAVE_DUNE_ISTL && HAVE_DUNE_FEM
// for dune-istl
#if defined(HAVE_BOOST)
#undef HAVE_BOOST
#define HAVE_BOOST 1
#endif
#include <dune/stuff/common/disable_warnings.hh>
#include <dune/fem/operator/matrix/spmatrix.hh>
#include <dune/fem/operator/matrix/istlmatrix.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/istl/operators.hh>
#include <dune/fem/operator/matrix/preconditionerwrapper.hh>
#endif // if HAVE_DUNE_ISTL && HAVE_DUNE_FEM


namespace Dune {
namespace Stuff {
namespace Common {

// \todo doc
template <class MatrixImp>
inline void clear(Dune::DenseMatrix<MatrixImp>& matrix)
{
  matrix *= typename Dune::DenseMatrix<MatrixImp>::value_type(0);
}


//! TODO
template <class MatrixImp>
struct PrecondionWrapperDummy
{
  typedef typename MatrixImp::RowDiscreteFunctionType::DofStorageType X;
  typedef typename MatrixImp::ColDiscreteFunctionType::DofStorageType Y;
  void pre(X&, Y&)
  {
  }
  void apply(X&, const Y&)
  {
  }
  void post(X&)
  {
  }
}; // struct PrecondionWrapperDummy


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

    for (size_t i = 0; i < FieldMatrixType::rows; i++)
      (*this)[i] = matrix[i][i];
  }
}; // class MatrixDiagonal

//! returns Sum of matrix' diagonal entries
template <class FieldMatrixType>
typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)
{
  MatrixDiagonal<FieldMatrixType> diag(matrix);
  typename FieldMatrixType::field_type trace = typename FieldMatrixType::field_type(0);
  for (size_t i = 0; i < FieldMatrixType::rows; i++)
    trace += diag[i];
  return trace;
} // typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)

//! produces a NxN Identity matrix compatible with parent type
template <class MatrixType>
class IdentityMatrix : public MatrixType
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

//! produces a NxN Identity matrix object compatible with parent type
template <class MatrixObjectType>
class IdentityMatrixObject : public MatrixObjectType
{
public:
  IdentityMatrixObject(const typename MatrixObjectType::DomainSpaceType& domain_space,
                       const typename MatrixObjectType::RangeSpaceType& range_space)
    : MatrixObjectType(domain_space, range_space)
  {
    MatrixObjectType::reserve();
    for (int i = 0; i < MatrixObjectType::matrix().rows(); ++i)
      MatrixObjectType::matrix().set(i, i, 1.0);
  }
};

//! adds the missing setDiag function to SparseRowMatrix
template <class DiscFuncType, class MatrixType>
void setMatrixDiag(MatrixType& matrix, DiscFuncType& diag)
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix
  DofIteratorType it = diag.dbegin();

  for (int row = 0; row < matrix.rows(); row++) {
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

  for (int row = 0; row < a.rows(); ++row) {
    for (int col = 0; col < a.cols(); ++col) {
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
  for (int i = 0; i < arg.rows(); ++i)
    for (int j = 0; j < arg.cols(); ++j) {
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
  const unsigned int nRows = matrix.rows();
  const unsigned int nCols = matrix.cols();

  for (int i = 0; i != nRows; ++i) {
    for (int j = 0; j != nCols; ++j) {
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
  for (int i = 0; i < arg.cols(); ++i)
    for (int j = 0; j < arg.rows(); ++j)
      dest.set(j, i, arg(i, j));
} // forceTranspose


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MATRIX_HH
