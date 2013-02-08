#ifndef DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH

#include <iostream>

// dune-common
#include <dune/common/densevector.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/deprecated.hh>

namespace Dune {
namespace Stuff {
namespace Common {

template <class VectorImp>
void clear(Dune::DenseVector<VectorImp>& vector)
{
  vector = typename Dune::DenseVector<VectorImp>::value_type(0);
} // void clear( DenseVectorType& vector )

template <class T>
DUNE_DEPRECATED_MSG("THIS DOES NOT RESIZE ANYTHING")
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
