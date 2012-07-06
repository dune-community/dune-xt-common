#ifndef DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH

namespace Dune {

namespace Stuff {

namespace Common {

namespace Vector {

template <class DenseVectorType>
void clear(DenseVectorType& vector)
{
  typedef typename DenseVectorType::value_type ValueType;

  const unsigned int size = vector.size();
  for (unsigned int i = 0; i < size; ++i) {
    vector[i] = ValueType(0);
  }
} // void clear( DenseVectorType& vector )

} // namespace Vector

} // namespace Common

} // namespace Stuff

} // namespace Dune

#endif // DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
