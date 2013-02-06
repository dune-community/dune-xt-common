#ifndef DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH

#include <iostream>

// dune-common
#include <dune/common/densevector.hh>
#include <dune/common/dynvector.hh>

namespace Dune {
namespace Stuff {
namespace Common {


// template< class T, class stream = std::ostream >
// void print(const T& arg, stream& out = std::cout, std::string name = "", std::string prefix = "") {
//  out << prefix;
//  if (!name.empty())
//    out << name << " = ";
//  out << "[";
//  for (unsigned int i = 0; i < (arg.size() - 1); ++i)
//  {
//      out << arg[i] << ", ";
//  }
//  out << arg[arg.size() - 1] << "]" << std::endl;
//}


template <class VectorImp>
void clear(Dune::DenseVector<VectorImp>& vector)
{
  vector = typename Dune::DenseVector<VectorImp>::value_type(0);
  //  typedef typename DenseVectorType::value_type
  //    ValueType;

  //  const unsigned int size = vector.size();
  //  for( unsigned int i = 0; i < size; ++i )
  //  {
  //    vector[i] = ValueType(0.0);
  //  }
} // void clear( DenseVectorType& vector )


template <class T>
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
