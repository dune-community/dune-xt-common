#ifndef DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH

#include <iostream>

#include <dune/common/densevector.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>
#include <dune/common/deprecated.hh>

namespace Dune {
namespace Stuff {
namespace Common {

template <class VectorImp>
void clear(Dune::DenseVector<VectorImp>& vector)
{
  vector = typename Dune::DenseVector<VectorImp>::value_type(0);
}


/**
 *  \brief Compare x and y component-wise for almost equality.
 *
 *  This implementation is taken from pymor: https://github.com/pyMor/pyMor/blob/master/src/pymor/tools/floatcmp.py
 *  Credit goes to Stephan Rave
 */
template <class Field, int size>
bool float_cmp(const Dune::FieldVector<Field, size>& x, const Dune::FieldVector<Field, size>& y, const Field atol,
               const Field rtol)
{
  unsigned int failure = 0;
  for (size_t ii = 0; ii < size; ++ii)
    if (std::abs(x[ii] - y[ii]) > atol + std::abs(y[ii]) * rtol)
      ++failure;
  return !bool(failure);
}

template <class Field, int size>
bool float_cmp(const Dune::FieldVector<Field, size>& x, const Dune::FieldVector<Field, size>& y, const Field atol)
{
  unsigned int failure = 0;
  for (size_t ii = 0; ii < size; ++ii)
    if (std::abs(x[ii] - y[ii]) > atol + std::abs(y[ii]) * atol)
      ++failure;
  return !bool(failure);
}

template <class T>
DUNE_DEPRECATED_MSG("THIS WILL BE REMOVED ONCE ExtendedParameterTree::getVector() IS PROPERLY IMPLEMENTED!")
Dune::DynamicVector<T> resize(const Dune::DynamicVector<T>& inVector, const size_t newSize, const T fill = T(0))
{
  Dune::DynamicVector<T> outVector(newSize);
  for (size_t ii = 0; ii < std::min(inVector.size(), newSize); ++ii)
    outVector[ii] = inVector[ii];
  for (size_t ii = std::min(inVector.size(), newSize); ii < newSize; ++ii)
    outVector[ii] = fill;
  return outVector;
}

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
