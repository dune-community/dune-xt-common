#ifndef DUNE_STUFF_COMMON_MATRIX_HH
#define DUNE_STUFF_COMMON_MATRIX_HH

// dune-common
#include <dune/common/densematrix.hh>

#ifdef THIS_WORKS
#include <dune/fem/operator/matrix/spmatrix.hh>
#include <dune/common/static_assert.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/math.hh>

#if HAVE_DUNE_ISTL
#include <dune/istl/operators.hh>
#include <dune/fem/operator/matrix/istlmatrix.hh>
#include <dune/fem/operator/matrix/preconditionerwrapper.hh>
#endif // if HAVE_DUNE_ISTL
#endif // THIS_WORKS

namespace Dune {

namespace Stuff {

namespace Common {

namespace Matrix {

// \todo doc
template <class MatrixImp>
void clear(Dune::DenseMatrix<MatrixImp>& matrix)
{
  // preparations
  const typename Dune::DenseMatrix<MatrixImp>::value_type zero(0.0);
  // loop over all rows
  for (unsigned int i = 0; i < matrix.rows(); ++i) {
    // get row
    typename Dune::DenseMatrix<MatrixImp>::row_reference row = matrix[i];
    // loop over all cols
    for (unsigned int j = 0; j < matrix.cols(); ++j) {
      row[j] = zero;
    } // loop over all cols
  } // loop over all rows
} // void clear(Dune::DenseMatrix< MatrixImp >& matrix)

#ifdef THIS_WORKS
// ! TODO
template <class MatrixImp>
struct PrecondionWrapperDummy : public Preconditioner<typename MatrixImp::RowDiscreteFunctionType::DofStorageType,
                                                      typename MatrixImp::ColDiscreteFunctionType::DofStorageType>
{
  typedef Preconditioner<typename MatrixImp::RowDiscreteFunctionType::DofStorageType,
                         typename MatrixImp::ColDiscreteFunctionType::DofStorageType> BaseType;
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
  enum
  {
    category = SolverCategory::sequential
  };
}; // struct PrecondionWrapperDummy

// ! obsolete,dysfunctional Matrixoperator
template <class MatrixType>
class SaneSparseRowMatrixOperator
{
  const MatrixType& object_;

public:
  typedef SaneSparseRowMatrixOperator<MatrixType> ThisType;
  SaneSparseRowMatrixOperator(const MatrixType& object)
    : object_(object)
  {
  }

  template <class DomainFunction, class RangeFunction>
  void operator()(const DomainFunction& arg, RangeFunction& dest) const
  {
    object_.apply(arg, dest);
  }

#ifdef USE_BFG_CG_SCHEME
  template <class VectorType, class IterationInfo>
  void multOEM(const VectorType* x, VectorType* ret, const IterationInfo& /*info*/) const
  {
    // call multOEM of the matrix
    object_.multOEM(x, ret);
  }

#endif // ifdef USE_BFG_CG_SCHEME

  template <class VectorType>
  void multOEM(const VectorType* x, VectorType* ret) const
  {
    // call multOEM of the matrix
    object_.multOEM(x, ret);
  }

  const ThisType& systemMatrix() const
  {
    return *this;
  }
}; // class SaneSparseRowMatrixOperator

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
    dune_static_assert(FieldMatrixType::rows == FieldMatrixType::cols, "Matrix must be square!");
    // CompileTimeChecker< FieldMatrixType::rows == FieldMatrixType::cols > matrix_must_be_square;

    for (size_t i = 0; i < FieldMatrixType::rows; i++)
      (*this)[i] = matrix[i][i];
  }
}; // class MatrixDiagonal

// ! returns Sum of matrix' diagonal entries
template <class FieldMatrixType>
typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)
{
  MatrixDiagonal<FieldMatrixType> diag(matrix);
  typename FieldMatrixType::field_type trace = typename FieldMatrixType::field_type(0);
  for (size_t i = 0; i < FieldMatrixType::rows; i++)
    trace += diag[i];
  return trace;
} // typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)

// ! produces a NxN Identity matrix compatible with parent type
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

// ! produces a NxN Identity matrix object compatible with parent type
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

// ! adds the missing setDiag function to SparseRowMatrix
template <class DiscFuncType, class MatrixType>
void setMatrixDiag(MatrixType& matrix, DiscFuncType& diag)
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;

  // ! we assume that the dimension of the functionspace of f is the same as
  // ! the size of the matrix
  DofIteratorType it = diag.dbegin();

  for (int row = 0; row < matrix.rows(); row++) {
    if (*it != 0.0)
      matrix.set(row, row, *it);
    ++it;
  }
  return;
} // setMatrixDiag

// ! return false if <pre>abs( a(row,col) - b(col,row) ) > tolerance<pre> for any col,row
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

// ! extern matrix addition that ignore 0 entries
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
      std::istringstream in(subs);
      if (entryLine.size() == 2) {
        in.precision(12);
        in.setf(std::ios::scientific);
      } else {
        in.precision(10);
        in.setf(std::ios::fixed);
      }
      in >> temp;
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

// ! prints actual memusage of matrix in kB
template <class MatrixType>
void printMemUsage(const MatrixType& matrix, std::ostream& stream, std::string name = "")
{
  long size = matrix.numberOfValues() * sizeof(typename MatrixType::Ttype) / 1024.f;

  stream << "matrix size " << name << "\t\t" << size << std::endl;
}

// ! prints actual memusage of matrixobject in kB
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
#endif // THIS_WORKS

} // namespace Matrix

} // namespace Common

} // namespace Stuff

} // namespace Dune

#endif // DUNE_STUFF_COMMON_MATRIX_HH

/** Copyright (c) 2012, Rene Milk    , Sven Kaulmann
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
