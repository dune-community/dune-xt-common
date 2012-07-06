#ifndef DUNE_HELPER_TOOLS_COMMON_VECTOR_HH
#define DUNE_HELPER_TOOLS_COMMON_VECTOR_HH

namespace Dune {

namespace HelperTools {

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
} // end function clear

} // end namespace Vector

} // namespace Common

} // namespace HelperTools

} // namespace Dune

#endif // DUNE_HELPER_TOOLS_COMMON_VEECTOR_HH
