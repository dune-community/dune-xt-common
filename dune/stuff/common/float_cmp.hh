// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_HH

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/float_cmp.hh>
#include <dune/common/fvector.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/dynvector.hh>

#include <dune/stuff/la/container/interfaces.hh>

#include <type_traits>

namespace Dune {
namespace Stuff {
namespace Common {
namespace FloatCmp {


#define DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(id)                                                                      \
  template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>                                 \
  typename std::enable_if<std::is_arithmetic<T>::value, bool>::type id(                                                \
      const T& first,                                                                                                  \
      const T& second,                                                                                                 \
      typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())       \
  {                                                                                                                    \
    return Dune::FloatCmp::id<T, style>(first, second, epsilon);                                                       \
  }                                                                                                                    \
                                                                                                                       \
  template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>                                 \
  typename std::enable_if<std::is_arithmetic<T>::value, bool>::type id(                                                \
      const std::vector<T>& first,                                                                                     \
      const std::vector<T>& second,                                                                                    \
      typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())       \
  {                                                                                                                    \
    assert(first.size() == second.size());                                                                             \
    for (size_t ii = 0; ii < first.size(); ++ii)                                                                       \
      if (!Dune::FloatCmp::id<T, style>(first[ii], second[ii], epsilon))                                               \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>                                 \
  bool id(const Dune::DynamicVector<T>& first,                                                                         \
          const Dune::DynamicVector<T>& second,                                                                        \
          typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())   \
  {                                                                                                                    \
    assert(first.size() == second.size());                                                                             \
    for (size_t ii = 0; ii < first.size(); ++ii)                                                                       \
      if (!Dune::FloatCmp::id<T, style>(first[ii], second[ii], epsilon))                                               \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  template <class T, int size, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>                       \
  bool id(const Dune::FieldVector<T, size>& first,                                                                     \
          const Dune::FieldVector<T, size>& second,                                                                    \
          typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())   \
  {                                                                                                                    \
    for (size_t ii = 0; ii < size; ++ii)                                                                               \
      if (!Dune::FloatCmp::id<T, style>(first[ii], second[ii], epsilon))                                               \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }                                                                                                                    \
  template <class F, class S, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>                        \
  bool id(const Stuff::LA::VectorInterface<F>& first,                                                                  \
          const Stuff::LA::VectorInterface<S>& second,                                                                 \
          typename Dune::FloatCmp::EpsilonType<typename F::ScalarType>::Type epsilon =                                 \
              Dune::FloatCmp::DefaultEpsilon<typename F::ScalarType, style>::value())                                  \
  {                                                                                                                    \
    assert(first.size() == second.size());                                                                             \
    for (size_t ii = 0; ii < first.size(); ++ii)                                                                       \
      if (!Dune::FloatCmp::id<typename F::ScalarType, style>(first[ii], second[ii], epsilon))                          \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }
// DUNE_STUFF_GENERATE_VECTOR_COMPARATOR

DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(eq)
DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(ne)
DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(gt)
DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(lt)
DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(ge)
DUNE_STUFF_GENERATE_VECTOR_COMPARATOR(le)

#undef DUNE_STUFF_GENERATE_VECTOR_COMPARATOR

template <class T, int size, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool vec_ne(const Dune::FieldVector<T, size>& first, const Dune::FieldVector<T, size>& second,
            typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  return !eq(first, second, epsilon);
}

} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FLOAT_CMP_HH
