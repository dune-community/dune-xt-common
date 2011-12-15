#ifndef STUFF_MATRIX_HH
#define STUFF_MATRIX_HH

#include <dune/fem/operator/matrix/spmatrix.hh>
#include <dune/common/static_assert.hh>
#include <dune/stuff/debug.hh>

#if HAVE_DUNE_ISTL
#include <dune/istl/operators.hh>
#include <dune/fem/operator/matrix/istlmatrix.hh>
#include <dune/fem/operator/matrix/preconditionerwrapper.hh>
#endif

namespace Dune {
//! TODO
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
};

template <class MatrixImp>
class SchurkomplementOperatorAdapter
    : public AssembledLinearOperator<MatrixImp, typename MatrixImp::RowDiscreteFunctionType::DofStorageType,
                                     typename MatrixImp::ColDiscreteFunctionType::DofStorageType>
{
  typedef SchurkomplementOperatorAdapter<MatrixImp> ThisType;
  typedef AssembledLinearOperator<MatrixImp, typename MatrixImp::RowDiscreteFunctionType::DofStorageType,
                                  typename MatrixImp::ColDiscreteFunctionType::DofStorageType> BaseType;

public:
  enum
  {
    category = SolverCategory::sequential
  };
  typedef MatrixImp MatrixType;
  typedef PrecondionWrapperDummy<MatrixType> PreconditionAdapterType;

  typedef typename MatrixType::RowDiscreteFunctionType RowDiscreteFunctionType;
  typedef typename MatrixType::ColDiscreteFunctionType ColumnDiscreteFunctionType;

  typedef typename RowDiscreteFunctionType::DiscreteFunctionSpaceType RowSpaceType;

  typedef typename ColumnDiscreteFunctionType::DiscreteFunctionSpaceType ColSpaceType;
  typedef ParallelScalarProduct<ColumnDiscreteFunctionType> ParallelScalarProductType;

  typedef typename RowDiscreteFunctionType::DofStorageType X;
  typedef typename ColumnDiscreteFunctionType::DofStorageType Y;

  //! export types
  typedef MatrixType matrix_type;
  typedef X domain_type;
  typedef Y range_type;
  typedef typename X::field_type field_type;

protected:
  MatrixType& matrix_;
  const RowSpaceType& rowSpace_;
  const ColSpaceType& colSpace_;

  ParallelScalarProductType scp_;

  PreconditionAdapterType preconditioner_;
  mutable double averageCommTime_;

public:
  //! constructor: just store a reference to a matrix
  SchurkomplementOperatorAdapter(const SchurkomplementOperatorAdapter& org)
    : matrix_(org.matrix_)
    , rowSpace_(org.rowSpace_)
    , colSpace_(org.colSpace_)
    , scp_(colSpace_)
    , preconditioner_(org.preconditioner_)
    , averageCommTime_(org.averageCommTime_)
  {
  }

  //! constructor: just store a reference to a matrix
  SchurkomplementOperatorAdapter(MatrixType& A, const RowSpaceType& rowSpace, const ColSpaceType& colSpace)
    : matrix_(A)
    , rowSpace_(rowSpace)
    , colSpace_(colSpace)
    , scp_(colSpace_)
    , preconditioner_(PreconditionAdapterType())
    , averageCommTime_(0.0)
  {
  }

  //! return communication time
  double averageCommTime() const
  {
    return averageCommTime_;
  }

  //! return reference to preconditioner
  PreconditionAdapterType& preconditionAdapter()
  {
    return preconditioner_;
  }
  const PreconditionAdapterType& preconditionAdapter() const
  {
    return preconditioner_;
  }

  //! return reference to preconditioner
  ParallelScalarProductType& scp()
  {
    return scp_;
  }
  const ParallelScalarProductType& scp() const
  {
    return scp_;
  }

  //! apply operator to x:  \f$ y = A(x) \f$

  //! apply operator to x:  \f$ y = A(x) \f$
  virtual void apply(const X& x, Y& y) const
  {
    // exchange data first
    communicate(x);

    // apply vector to matrix
    matrix_.multOEM(x, y);

    // delete non-interior
    scp_.deleteNonInterior(y);
  }

  //! apply operator to x, scale and add:  \f$ y = y + \alpha A(x) \f$
  virtual void applyscaleadd(field_type alpha, const X& x, Y& y) const
  {
    // exchange data first
    communicate(x);

    // apply matrix
    matrix_.usmv(alpha, x, y);

    // delete non-interior
    scp_.deleteNonInterior(y);
  }

  template <class T, class O>
  double residuum(const T& rhs, O& x) const
  {
    // exchange data
    communicate(x);
    T tmp(rhs);
    apply(x, tmp);
    tmp -= rhs;
    double res = tmp.two_norm();

    res = rowSpace_.grid().comm().sum(res);
    // return global sum of residuum
    return std::sqrt(res);
  }

  //! get matrix via *
  virtual const MatrixType& getmat() const
  {
    return matrix_;
  }

protected:
  void communicate(const X& x) const
  {

    if (rowSpace_.grid().comm().size() <= 1)
      return;

    Timer commTime;

    // create temporary discretet function object
    RowDiscreteFunctionType tmp("DGParallelMatrixAdapter::communicate", rowSpace_, x);

    // exchange data by copying
    rowSpace_.communicate(tmp);

    // accumulate communication time
    averageCommTime_ += commTime.elapsed();
  }
};

//! obsolete,dysfunctional Matrixoperator
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
    dune_static_assert(FieldMatrixType::rows == FieldMatrixType::cols, "Matrix must be square!");
    // CompileTimeChecker< FieldMatrixType::rows == FieldMatrixType::cols > matrix_must_be_square;

    for (size_t i = 0; i < FieldMatrixType::rows; i++)
      (*this)[i] = matrix[i][i];
  }
};

//! returns Sum of matrix' diagonal entries
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

  const MatrixType& matrix() const
  {
    return *this;
  }
};

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
    //			dune_static_assert( true );
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
}

//! return false if <pre>abs( a(row,col) - b(col,row) ) > tolerance<pre> for any col,row
template <class MatrixType>
bool areTransposed(const MatrixType& a, const MatrixType& b, const double tolerance = 1e-8)
{
  if (a.rows() != b.cols() || b.rows() != a.cols())
    return false;
  for (int row = 0; row < a.rows(); ++row) {
    for (int col = 0; col < a.cols(); ++col) {
      if (std::fabs(a(row, col) - b(col, row)) > tolerance)
        return false;
    }
  }
  return true;
}
} // namespace Dune


namespace Stuff {
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
}

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
}

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
}

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
}


namespace Matrix {

//! prints actual memusage of matrix in kB
template <class MatrixType, class Stream>
void printMemUsage(const MatrixType& matrix, Stream& stream, std::string name = "")
{
  long size = matrix.numberOfValues() * sizeof(typename MatrixType::Ttype) / 1024.f;
  stream << "matrix size " << name << "\t\t" << size << std::endl;
}

//! prints actual memusage of matrixobject in kB
template <class MatrixObjectType, class Stream>
void printMemUsageObject(const MatrixObjectType& matrix_object, Stream& stream, std::string name = "")
{
  printMemUsage(matrix_object.matrix(), stream, name);
}
#if !STOKES_USE_ISTL /// TODO
//! a small proxy object that automagically prevents near-0 value fill-in
template <class MatrixPointerType>
class LocalMatrixProxy
{
  typedef typename MatrixPointerType::element_type MatrixObjectType;
  typedef typename MatrixObjectType::LocalMatrixType LocalMatrixType;
  typedef typename MatrixObjectType::DomainSpaceType::GridType GridType;
  typedef typename GridType::template Codim<0>::Entity EntityType;
  typedef typename MatrixObjectType::MatrixType::Ttype FieldType;
  LocalMatrixType local_matrix_;
  const double eps_;
  const unsigned int rows_;
  const unsigned int cols_;
  std::vector<FieldType> entries_;

public:
  LocalMatrixProxy(MatrixPointerType& object, const EntityType& self, const EntityType& neigh, const double eps)
    : local_matrix_(object->localMatrix(self, neigh))
    , eps_(eps)
    , rows_(local_matrix_.rows())
    , cols_(local_matrix_.columns())
    , entries_(rows_ * cols_, FieldType(0.0))
  {
  }

  inline void add(const unsigned int row, const unsigned int col, const FieldType val)
  {
    ASSERT_LT(row, rows_);
    ASSERT_LT(col, cols_);
    entries_[row * cols_ + col] += val;
  }

  ~LocalMatrixProxy()
  {
    for (unsigned int i = 0; i < rows_; ++i) {
      for (unsigned int j = 0; j < cols_; ++j) {
        const FieldType& i_j = entries_[i * cols_ + j];
        if (std::fabs(i_j) > eps_)
          local_matrix_.add(i, j, i_j);
      }
    }
  }
  unsigned int rows() const
  {
    return rows_;
  }
  unsigned int cols() const
  {
    return cols_;
  }
};
#else
//! a small proxy object that automagically prevents near-0 value fill-in
template <class MatrixPointerType>
class LocalMatrixProxy
{
  typedef typename MatrixPointerType::element_type MatrixObjectType;
  typedef typename MatrixObjectType::LocalMatrixType LocalMatrixType;
  typedef typename MatrixObjectType::MatrixType::block_type block_type;
  typedef typename GridType::template Codim<0>::Entity EntityType;
  typedef typename MatrixObjectType::MatrixType::Ttype FieldType;
  //			LocalMatrixType local_matrix_;
  MatrixPointerType matrix_pointer_;
  const EntityType& self_;
  const EntityType& neigh_;
  const double eps_;
  const unsigned int rows_;
  const unsigned int cols_;
  std::vector<FieldType> entries_;
  //! global row numbers
  std::vector<int> rowMap_;
  //! global col numbers
  std::vector<int> colMap_;


public:
  LocalMatrixProxy(MatrixPointerType& pointer, const EntityType& self, const EntityType& neigh, const double eps)
    : // local_matrix_( object->localMatrix(self,neigh) ),
    matrix_pointer_(pointer)
    , self_(self)
    , neigh_(neigh)
    , eps_(eps)
    , rows_(block_type::rows)
    , cols_(block_type::cols)
    , entries_(rows_ * cols_, FieldType(0.0))
  {
    const auto& domainSpace = matrix_pointer_->rowSpace();
    const auto& rangeSpace = matrix_pointer_->colSpace();
    rowMap_.resize(domainSpace.baseFunctionSet(self).numBaseFunctions());
    colMap_.resize(rangeSpace.baseFunctionSet(neigh).numBaseFunctions());

    const auto dmend = domainSpace.mapper().end(self);
    for (auto dmit = domainSpace.mapper().begin(self); dmit != dmend; ++dmit) {
      assert(dmit.global() == domainSpace.mapToGlobal(self, dmit.local()));
      rowMap_[dmit.local()] = dmit.global();
    }
    const auto rmend = rangeSpace.mapper().end(neigh);
    for (auto rmit = rangeSpace.mapper().begin(neigh); rmit != rmend; ++rmit) {
      assert(rmit.global() == rangeSpace.mapToGlobal(neigh, rmit.local()));
      colMap_[rmit.local()] = rmit.global();
    }
  }

  inline void add(const unsigned int row, const unsigned int col, const FieldType val)
  {
    ASSERT_LT(row, rows_);
    ASSERT_LT(col, cols_);
    entries_[row * cols_ + col] += val;
  }

  ~LocalMatrixProxy()
  {
    for (unsigned int i = 0; i < rows_; ++i) {
      for (unsigned int j = 0; j < cols_; ++j) {
        const FieldType& i_j = entries_[i * cols_ + j];
        if (std::fabs(i_j) > eps_) {
          matrix_pointer_->add(rowMap_[i], colMap_[j], i_j);
        }
      }
    }
  }

  unsigned int rows() const
  {
    return rows_;
  }
  unsigned int cols() const
  {
    return cols_;
  }
};
#endif
template <class M>
void forceTranspose(const M& arg, M& dest)
{
  assert(arg.cols() == dest.rows());
  assert(dest.cols() == arg.rows());
  //			dest.clear();
  for (int i = 0; i < arg.cols(); ++i)
    for (int j = 0; j < arg.rows(); ++j)
      dest.set(j, i, arg(i, j));
}
}
} // namespace Stuff

#endif // MATRIX_HH
