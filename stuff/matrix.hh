#ifndef STUFF_MATRIX_HH
#define STUFF_MATRIX_HH

#include <dune/fem/operator/matrix/spmatrix.hh>

namespace Dune {
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
  void multOEM(const VectorType* x, VectorType* ret, const IterationInfo& info) const
  {
    // call multOEM of the matrix
    object_.multOEM(x, ret);
  }
#endif

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
};

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
    CompileTimeChecker<FieldMatrixType::rows == FieldMatrixType::cols> matrix_must_be_square;
    for (size_t i = 0; i < FieldMatrixType::rows; i++)
      (*this)[i] = matrix[i][i];
  }
};

//! YAYDDYA
template <class FieldMatrixType>
typename FieldMatrixType::field_type matrixTrace(const FieldMatrixType& matrix)
{
  MatrixDiagonal<FieldMatrixType> diag(matrix);
  typename FieldMatrixType::field_type trace = typename FieldMatrixType::field_type(0);
  for (size_t i = 0; i < FieldMatrixType::rows; i++)
    trace += diag[i];
  return trace;
}

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
};

//! adds the missing setDiag fucntion to SparseRowMatrix
template <class DiscFuncType, class MatrixType>
void setMatrixDiag(MatrixType& matrix, DiscFuncType& diag)
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix
  DofIteratorType it = diag.dbegin();

  for (int row = 0; row < matrix.size(0); row++) {
    if (*it != 0.0)
      matrix.set(row, row, *it);
    ++it;
  }
  return;
}

template <class MatrixType>
bool areTransposed(const MatrixType& a, const MatrixType& b, const double tolerance = 1e-8)
{
  if (a.rows() != b.cols() || b.rows() != a.cols())
    return false;
  for (int row = 0; row < a.rows(); ++row) {
    for (int col = 0; col < a.cols(); ++col) {
      if (std::abs(a(row, col) - b(col, row)) > tolerance)
        return false;
    }
  }
  return true;
}
} // namespace Dune


#endif // MATRIX_HH
