// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_HH

#include <type_traits>
#include <complex>

#include <dune/common/float_cmp.hh>

#include <dune/stuff/common/type_utils.hh>
#include "float_cmp_style.hh"
#include "float_cmp_internal.hh"

namespace Dune {
namespace Stuff {
namespace Common {


// forwards (include is below)
template <class VecType>
struct VectorAbstraction;

template <class VectorType>
struct is_vector;


namespace FloatCmp {

template <class T, Style style = Style::defaultStyle>
struct DefaultEpsilon
{
  typedef typename Dune::FloatCmp::EpsilonType<T>::Type Type;
  static Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, internal::ConvertStyle<style>::value>::value();
  }
};

//! we use the scalar field's type from std::complex for its epsilon
template <class T, Style style>
struct DefaultEpsilon<std::complex<T>, style>
{
  typedef typename Dune::FloatCmp::EpsilonType<T>::Type Type;
  static Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, internal::ConvertStyle<style>::value>::value();
  }
};

template <class T>
struct DefaultEpsilon<T, Style::numpy>
{
  typedef typename Dune::FloatCmp::EpsilonType<T>::Type Type;
  static Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, Dune::FloatCmp::relativeStrong>::value();
  }
};

template <class T>
struct DefaultEpsilon<std::complex<T>, Style::numpy>
{
  typedef typename Dune::FloatCmp::EpsilonType<T>::Type Type;
  static Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, Dune::FloatCmp::relativeStrong>::value();
  }
};


#define DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(id)                                                                      \
  template <Style style,                                                                                               \
            class FirstType,                                                                                           \
            class SecondType,                                                                                          \
            class ToleranceType = typename VectorAbstraction<FirstType>::R>                                            \
  typename std::enable_if<internal::cmp_type_check<FirstType, SecondType, ToleranceType>::value, bool>::type id(       \
      const FirstType& first,                                                                                          \
      const SecondType& second,                                                                                        \
      const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& rtol =                                          \
          DefaultEpsilon<ToleranceType, style>::value(),                                                               \
      const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& atol =                                          \
          DefaultEpsilon<ToleranceType, style>::value())                                                               \
  {                                                                                                                    \
    return internal::Call<FirstType, SecondType, typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type, style>::   \
        id(first, second, rtol, atol);                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  template <class FirstType, class SecondType, class ToleranceType = typename VectorAbstraction<FirstType>::R>         \
  typename std::enable_if<internal::cmp_type_check<FirstType, SecondType, ToleranceType>::value, bool>::type id(       \
      const FirstType& first,                                                                                          \
      const SecondType& second,                                                                                        \
      const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& rtol =                                          \
          DefaultEpsilon<ToleranceType, Style::defaultStyle>::value(),                                                 \
      const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& atol =                                          \
          DefaultEpsilon<ToleranceType, Style::defaultStyle>::value())                                                 \
  {                                                                                                                    \
    return id<Style::defaultStyle>(first, second, rtol, atol);                                                         \
  }


DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(eq)
DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(ne)
DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(gt)
DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(lt)
DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(ge)
DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(le)
#undef DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR

} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#include <dune/stuff/common/vector.hh>

#endif // DUNE_STUFF_COMMON_FLOAT_CMP_HH
