#ifndef DUNESTUFF_PRINTING_HH_INCLUDED
#define DUNESTUFF_PRINTING_HH_INCLUDED

#include <string>
#include <iostream>
#include <iomanip>

namespace Stuff {

/**
 *  \brief prints a Dune::FieldVector
 *
 *  or anything compatible in terms of Iterators
 *  \tparam T
 *          should be Dune::FieldVector or compatible
 *  \tparam stream
 *          std::ostream or compatible
 *  \param[in]  arg
 *          vector to be printed
 *  \param[in]  name
 *          name to be printed along
 *  \param[in]  out
 *          where to print
 *  \param[opt] prefix
 *          prefix to be printed before every line
 **/
template <class T, class stream>
void printFieldVector(T& arg, std::string name, stream& out, std::string prefix = "")
{
  out << "\n" << prefix << "printing " << name << " (Dune::FieldVector)" << std::endl;
  typedef typename T::ConstIterator IteratorType;
  IteratorType itEnd = arg.end();
  out << prefix;
  for (IteratorType it = arg.begin(); it != itEnd; ++it) {
    out << std::setw(10) << std::setprecision(6) << *it;
  }
}

/**
 *  \brief prints a Dune::FieldMatrix
 *
 *  or anything compatible in terms of Iterators
 *  \tparam T
 *          should be Dune::FieldVector or compatible
 *  \tparam stream
 *          std::ostream or compatible
 *  \param[in]  arg
 *          matrix to be printed
 *  \param[in]  name
 *          name to be printed along
 *  \param[in]  out
 *          where to print
 *  \param[opt] prefix
 *          prefix to be printed before every line
 **/
template <class T, class stream>
void printFieldMatrix(T& arg, std::string name, stream& out, std::string prefix = "")
{
  out << "\n" << prefix << "printing " << name << " (Dune::FieldMatrix)";
  typedef typename T::ConstRowIterator RowIteratorType;
  typedef typename T::row_type::ConstIterator VectorInRowIteratorType;
  unsigned int row       = 1;
  RowIteratorType rItEnd = arg.end();
  for (RowIteratorType rIt = arg.begin(); rIt != rItEnd; ++rIt) {
    out << "\n" << prefix << "  row " << row << ":";
    VectorInRowIteratorType vItEnd = rIt->end();
    for (VectorInRowIteratorType vIt = rIt->begin(); vIt != vItEnd; ++vIt) {
      out << std::setw(10) << std::setprecision(6) << *vIt;
    }
    row += 1;
  }
}

template <class T, class stream>
void printSparseRowMatrixMatlabStyle(const T& arg, const std::string name, stream& out)
{
  out << "\n" << name << " = [ " << std::endl;
  for (int row = 0; row < arg.rows(); row++) {
    for (int col = 0; col < arg.cols(); col++) {
      out << std::setw(19) << std::setprecision(12) << arg(row, col);
    }
    out << ";" << std::endl;
  }
  out << "];" << std::endl;
}

template <class T, class stream>
void printDiscreteFunctionMatlabStyle(const T& arg, const std::string name, stream& out)
{
  out << "\n" << name << " = [ " << std::endl;
  typedef typename T::ConstDofIteratorType ConstDofIteratorType;
  ConstDofIteratorType itEnd = arg.dend();
  for (ConstDofIteratorType it = arg.dbegin(); it != itEnd; ++it) {
    out << std::setprecision(12) << *it;
    out << ";" << std::endl;
  }
  out << "];" << std::endl;
}

template <class T, class stream>
void printDoubleVectorMatlabStyle(const T* arg, const int size, const std::string name, stream& out)
{
  out << "\n" << name << " = [ " << std::endl;
  for (unsigned int i = 0; i < size; i++) {
    out << std::setprecision(12) << arg[i];
    out << ";" << std::endl;
  }
  out << "];" << std::endl;
}


template <class Stream, class Type>
void printDoubleVec(Stream& stream, const Type* vec, const unsigned int N)
{
  stream << "\n [ " << std::setw(5);
  for (unsigned int i = 0; i < N; ++i)
    stream << vec[i] << " ";

  stream << " ] " << std::endl;
}

template <class Stream, class DiscFunc>
void oneLinePrint(Stream& stream, const DiscFunc& func)
{
  typedef typename DiscFunc::ConstDofIteratorType DofIteratorType;
  DofIteratorType it = func.dbegin();
  stream << "\n" << func.name() << ": \n[ ";
  for (; it != func.dend(); ++it) {
    // double d = 0.10;// + *it; //stupid hack cause setw/prec ain't working for me
    stream << std::setw(6) << std::setprecision(3) << *it << "  ";
  }

  stream << " ] " << std::endl;
}


template <class GlobalMatrix, class Stream>
class LocalMatrixPrintFunctor
{
public:
  LocalMatrixPrintFunctor(const GlobalMatrix& m, Stream& stream, const std::string name)
    : matrix_(m)
    , stream_(stream)
    , name_(name)
  {
  }

  template <class Entity>
  void operator()(const Entity& en, const Entity& ne, const int en_idx, const int ne_idx)
  {
    typename GlobalMatrix::LocalMatrixType localMatrix = matrix_.localMatrix(en, ne);
    const int rows                                     = localMatrix.rows();
    const int cols                                     = localMatrix.columns();
    stream_ << "\nlocal_" << name_ << "_Matrix_" << en_idx << "_" << ne_idx << " = [" << std::endl;
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        stream_ << std::setw(8) << std::setprecision(2) << localMatrix.get(i, j);
      }
      stream_ << ";" << std::endl;
    }
    stream_ << "];" << std::endl;
  }

  void preWalk()
  {
    stream_ << "% printing local matrizes of " << name_ << std::endl;
  }

  void postWalk()
  {
    stream_ << "\n% done printing local matrizes of " << name_ << std::endl;
  }

private:
  const GlobalMatrix& matrix_;
  Stream& stream_;
  const std::string name_;
};


} // end namespace
#endif
