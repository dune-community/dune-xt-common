/**
 *  \file   stuff.hh
 *  \brief  contains some stuff
 **/
#ifndef STUFF_HH_INCLUDED
#define STUFF_HH_INCLUDED

#define SEGFAULT                                                                                                       \
  {                                                                                                                    \
    int* J = 0;                                                                                                        \
    *J     = 9;                                                                                                        \
  }
#define isnan(x) !(x == x)

#ifndef NDEBUG
#ifndef LOGIC_ERROR
#include <stdexcept>
#include <sstream>
#define LOGIC_ERROR                                                                                                    \
  {                                                                                                                    \
    std::stringstream ss;                                                                                              \
    ss << __FILE__ << ":" << __LINE__ << " should never be called";                                                    \
    throw std::logic_error(ss.str());                                                                                  \
  }
#endif
#else
#define LOGIC_ERROR
#endif

#include <iomanip>
#include <vector>
#include <cmath>

namespace Stuff {

/**
 *  \todo doc me
 **/
template <class ReturnType>
ReturnType fromString(const std::string& s)
{
  std::stringstream ss;
  ss << s;
  ReturnType r;
  ss >> r;
  return r;
}

/**
 *  \todo doc
 **/
template <class ReturnType>
std::string toString(const ReturnType& s)
{
  std::stringstream ss;
  ss << s;
  std::string r;
  ss >> r;
  return r;
}

template <class Info>
class TexOutput
{
  typedef std::vector<std::string> Strings;

  Info info_;
  double current_h_;
  Strings headers_;


public:
  TexOutput(const Info& info, Strings& headers)
    : info_(info)
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  TexOutput(Strings& headers)
    : info_(Info())
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  void setInfo(const Info& info)
  {
    info_ = info;
  }

  void putLineEnd(std::ofstream& outputFile_)
  {
    outputFile_ << "\n"
                << "\\tabularnewline\n"
                << "\\hline \n";
    outputFile_.flush();
  }

  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool initial)
  {
    current_h_    = info_.grid_width;
    double factor = prevh_ / current_h_;
    double eoc    = log(prevError_ / error_) / log(factor);
    outputFile_ << " & " << error_ << " & " << eoc;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";
    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << "|c|";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\hline \n";
    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << headers_[i];
      if (i < statColSize - 1)
        outputFile_ << " & ";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << " & " << headers_[i + statColSize] << " & EOC ";
    }
    outputFile_ << "\n \\endhead\n"
                << "\\hline\n"
                << "\\hline\n";
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << info_.c11 << " & " << info_.d11
                << " & " << info_.c12 << " & " << info_.d12;
  }

  void endTable(std::ofstream& outputFile_)
  {
    outputFile_ << "\\end{longtable}";
    outputFile_.flush();
  }

  double get_h()
  {
    return current_h_;
  }
};

/**
 *  \brief Only free mem pointed to by valid pointer, log warning otherwise
 *
 **/

template <class T>
void safe_delete(T t)
{
  if (t) {
    delete t;
    t = 0;
  }
  // else log warning
}

template <class Matrix, class Function>
void DiagonalMult(const Matrix& matrix, Function& f)
{
  Function diag("temp", f.space());
  matrix.getDiag(diag);

  typedef typename Function::DofIteratorType DofIteratorType;
  DofIteratorType diag_it = diag.dbegin();
  DofIteratorType f_it    = f.dbegin();

  for (int row = 0; row < matrix.size(0); row++) {
    (*f_it) *= (*diag_it);
    ++f_it;
    ++diag_it;
  }
  return;
}


/**
 *  \brief  gets min and max of a Dune::DiscreteFunction
 *
 *          or compatible in terms of iterators
 *  \todo   doc
 *  \attention  works only for constant base function = sqrt(2) atm
 **/
template <class FunctionType>
void getMinMaxOfDiscreteFunction(const FunctionType& function, double& min, double& max)
{
  // preparations
  min = 0.0;
  max = 0.0;
  typedef typename FunctionType::ConstDofIteratorType ConstDofIteratorType;
  ConstDofIteratorType itEnd = function.dend();
  // find minimum and maximum
  for (ConstDofIteratorType it = function.dbegin(); it != itEnd; ++it) {
    min = *it < min ? *it : min;
    max = *it > max ? *it : max;
  }
}

template <class FunctionType>
unsigned int getNumDiffDofs(const FunctionType& f1, const FunctionType& f2, const double tolerance)
{
  assert(f1.size() == f2.size()); // should be implicit cause their of same type...
  unsigned int numDiffs = 0;
  typedef typename FunctionType::ConstDofIteratorType ConstDofIteratorType;
  ConstDofIteratorType itEnd = f1.dend();
  ConstDofIteratorType f2it  = f2.dbegin();
  // find minimum and maximum
  for (ConstDofIteratorType f1it = f1.dbegin(); f1it != itEnd; ++f1it, ++f2it) {
    numDiffs += (std::fabs(*f1it - *f2it) > tolerance);
  }
  return numDiffs;
}

template <class Function>
void addScalarToFunc(Function& f, double sc)
{
  typedef typename Function::DofIteratorType DofIteratorType;
  DofIteratorType it = f.dbegin();
  for (; it != f.dend(); ++it)
    *it += sc;
  return;
}

template <class Function>
double getFuncAvg(const Function& f)
{
  typedef typename Function::ConstDofIteratorType DofIteratorType;
  DofIteratorType it         = f.dbegin();
  const unsigned int numdofs = f.size();
  double sum = 0;
  for (; it != f.dend(); ++it)
    sum += *it;
  sum /= double(numdofs);
  return sum;
}

template <class Function>
void switchDofs(Function& f)
{
  typedef typename Function::DofIteratorType DofIteratorType;
  DofIteratorType front      = f.dbegin();
  DofIteratorType back       = f.dend();
  const unsigned int numdofs = f.size();
  for (unsigned int i = 0; i < numdofs / 2; ++i) {
    double tmp = *back;
    *back      = *front;
    *front     = tmp;
  }
  return;
}


/**
 *  \brief  calculates length of given intersection in world coordinates
 *  \tparam IntersectionIteratorType
 *          IntersectionIteratorType
 *  \param[in]  intIt
 *          intersection
 *  \return length of intersection
 **/
template <class IntersectionIteratorType>
double getLenghtOfIntersection(const IntersectionIteratorType& intIt)
{
  typedef typename IntersectionIteratorType::Geometry IntersectionGeometryType;
  const IntersectionGeometryType& intersectionGeoemtry = intIt.intersectionGlobal();
  assert(intersectionGeoemtry.corners() == 2);
  typedef typename IntersectionIteratorType::ctype ctype;
  const int dimworld = IntersectionIteratorType::dimensionworld;
  typedef Dune::FieldVector<ctype, dimworld> DomainType;
  const DomainType cornerOne  = intersectionGeoemtry[0];
  const DomainType cornerTwo  = intersectionGeoemtry[1];
  const DomainType difference = cornerOne - cornerTwo;
  return difference.two_norm();
}

template <class Container, class Element>
int getIdx(const Container& ct, Element e)
{
  int idx = 0;
  for (typename Container::const_iterator it = ct.begin(); it != ct.end(); ++it, ++idx) {
    if (*it == e)
      return idx;
  }
  return -1;
}

template <class Space, int codim = 0>
class GridWalk
{
private:
  typedef typename Space::GridPartType GridPart;
  typedef typename GridPart::template Codim<codim>::IteratorType EntityIteratorType;
  typedef typename GridPart::IntersectionIteratorType IntersectionIteratorType;
  typedef typename IntersectionIteratorType::EntityPointer EntityPointer;

public:
  GridWalk(Space& gp)
    : space_(gp)
    , gridPart_(gp.gridPart())
  {
    EntityIteratorType entityItEndLog = space_.end();
    unsigned int en_idx = 0;
    for (EntityIteratorType it = space_.begin(); it != entityItEndLog; ++it, ++en_idx) {
      entityIdxMap_.push_back(it);
    }
  }
  template <class Functor>
  void operator()(Functor& f)
  {
    f.preWalk();
    EntityIteratorType entityItEndLog = space_.end();
    for (EntityIteratorType it = space_.begin(); it != entityItEndLog; ++it) {
      int ent_idx = getIdx(entityIdxMap_, it);
      f(*it, *it);
      IntersectionIteratorType intItEnd = gridPart_.iend(*it);
      for (IntersectionIteratorType intIt = gridPart_.ibegin(*it); intIt != intItEnd; ++intIt) {
        int neigh_idx = getIdx(entityIdxMap_, intIt.outside());
        f(*it, *intIt.outside());
      }
    }
    f.postWalk();
  }

private:
  Space& space_;
  GridPart& gridPart_;
  typedef std::vector<EntityPointer> EntityIdxMap;
  EntityIdxMap entityIdxMap_;
};

template <class GlobalMatrix, class Stream>
class LocalMatrixPrintFunctor
{
public:
  LocalMatrixPrintFunctor(const GlobalMatrix& m, Stream& stream)
    : matrix_(m)
    , stream_(stream)
  {
  }

  template <class Entity>
  void operator()(const Entity& en, const Entity& ne)
  {
    typename GlobalMatrix::LocalMatrixType localMatrix = matrix_.localMatrix(en, ne);
  }

  void preWalk()
  {
  }

  void postWalk()
  {
  }

private:
  const GlobalMatrix& matrix_;
  Stream& stream_;
};

} // end namepspace stuff


#endif // end of stuff.hh
