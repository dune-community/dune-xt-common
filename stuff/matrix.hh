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

} // namespace Dune


#endif // MATRIX_HH
