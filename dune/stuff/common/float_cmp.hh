// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_HH

#include <type_traits>
#include <complex>

#include <dune/common/float_cmp.hh>

namespace Dune {
namespace Stuff {
namespace Common {

namespace internal {


template <class Tt>
struct is_complex_helper
{
  DSC_has_typedef_initialize_once(value_type)

      static const bool is_candidate = DSC_has_typedef(value_type)<Tt>::value;
}; // class is_complex_helper


} // namespace internal


template <class T, bool candidate = internal::is_complex_helper<T>::is_candidate>
struct is_complex : public std::is_base_of<std::complex<typename T::value_type>, T>
{
};


template <class T>
struct is_complex<T, false> : public std::false_type
{
};

// forwards (include is below)
template <class VecType>
struct VectorAbstraction;

template <class VectorType>
struct is_vector;


namespace FloatCmp {


enum class Style
{
  numpy,
  relativeWeak,
  relativeStrong,
  absolute,
  defaultStyle = numpy
};


namespace internal {


template <Style style>
struct ConvertStyle;

template <>
struct ConvertStyle<Style::relativeWeak>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeWeak;
};

template <>
struct ConvertStyle<Style::relativeStrong>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeStrong;
};

template <>
struct ConvertStyle<Style::absolute>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::absolute;
};


} // namespace internal


template <class T, Style style = Style::defaultStyle>
struct DefaultEpsilon
{
  static typename Dune::FloatCmp::EpsilonType<T>::Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, internal::ConvertStyle<style>::value>::value();
  }
};

template <class T>
struct DefaultEpsilon<T, Style::numpy>
{
  static typename Dune::FloatCmp::EpsilonType<T>::Type value()
  {
    return Dune::FloatCmp::DefaultEpsilon<T, Dune::FloatCmp::relativeStrong>::value();
  }
};


namespace internal {


/**
 * Taken from http://docs.pymor.org/en/latest/_modules/pymor/tools/floatcmp.html#float_cmp,
 *            https://github.com/numpy/numpy/blob/v1.9.1/numpy/core/numeric.py#L2238
 */
template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type float_cmp(const T& xx, const T& yy, const T& rtol,
                                                                            const T& atol)
{
  return std::abs(xx - yy) <= atol + std::abs(yy) * rtol;
}


template <class T>
typename std::enable_if<is_complex<T>::value, bool>::type
float_cmp(const T& xx, const T& yy, const typename T::value_type& rtol, const typename T::value_type& atol)
{
  return (float_cmp(std::real(xx), std::real(yy), rtol, atol) && float_cmp(std::imag(xx), std::imag(yy), rtol, atol));
}


template <class XType, class YType, class TolType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value && std::is_arithmetic<TolType>::value
                            && std::is_same<typename VectorAbstraction<XType>::R, TolType>::value
                            && std::is_same<typename VectorAbstraction<YType>::R, TolType>::value,
                        bool>::type
float_cmp(const XType& xx, const YType& yy, const TolType& rtol, const TolType& atol)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!float_cmp(xx[ii], yy[ii], rtol, atol))
      return false;
  return true;
} // ... float_cmp(...)


template <Dune::FloatCmp::CmpStyle style, class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type dune_float_cmp(const T& xx, const T& yy, const T& eps)
{
  return Dune::FloatCmp::eq<T, style>(xx, yy, eps);
}

template <Dune::FloatCmp::CmpStyle style, class XType, class YType, class EpsType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value && std::is_arithmetic<EpsType>::value
                            && std::is_same<typename VectorAbstraction<XType>::R, EpsType>::value
                            && std::is_same<typename VectorAbstraction<YType>::R, EpsType>::value,
                        bool>::type
dune_float_cmp(const XType& xx, const YType& yy, const EpsType& eps)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!dune_float_cmp<style>(xx[ii], yy[ii], eps))
      return false;
  return true;
} // ... dune_float_cmp(...)


template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type cmp_gt(const T& xx, const T& yy)
{
  return xx > yy;
}

template <class XType, class YType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value
                            && std::is_same<typename VectorAbstraction<XType>::S,
                                            typename VectorAbstraction<YType>::S>::value,
                        bool>::type
cmp_gt(const XType& xx, const YType& yy)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!(xx[ii] > yy[ii]))
      return false;
  return true;
} // ... cmp_gt(...)


template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type cmp_lt(const T& xx, const T& yy)
{
  return xx < yy;
}

template <class XType, class YType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value
                            && std::is_same<typename VectorAbstraction<XType>::S,
                                            typename VectorAbstraction<YType>::S>::value,
                        bool>::type
cmp_lt(const XType& xx, const YType& yy)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!(xx[ii] < yy[ii]))
      return false;
  return true;
} // ... cmp_lt(...)


template <class FirstType, class SecondType, class ToleranceType, Style style>
struct Call
{
  static bool eq(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol);
  }

  static bool ne(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol);
  }

  static bool gt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol) && cmp_gt(first, second);
  }

  static bool lt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol) && cmp_lt(first, second);
  }

  static bool ge(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol) || cmp_gt(first, second);
  }

  static bool le(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp<internal::ConvertStyle<style>::value>(first, second, rtol) || cmp_lt(first, second);
  }
};

template <class FirstType, class SecondType, class ToleranceType>
struct Call<FirstType, SecondType, ToleranceType, Style::numpy>
{
  static bool eq(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp(first, second, rtol, atol);
  }

  static bool ne(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp(first, second, rtol, atol);
  }

  static bool gt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp(first, second, rtol, atol) && cmp_gt(first, second);
  }

  static bool lt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp(first, second, rtol, atol) && cmp_lt(first, second);
  }

  static bool ge(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp(first, second, rtol, atol) || cmp_gt(first, second);
  }

  static bool le(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp(first, second, rtol, atol) || cmp_lt(first, second);
  }
};


} // namespace internal


#define DUNE_STUFF_COMMON_FLOAT_CMP_GENERATOR(id)                                                                      \
  template <Style style,                                                                                               \
            class FirstType,                                                                                           \
            class SecondType,                                                                                          \
            class ToleranceType = typename VectorAbstraction<FirstType>::R>                                            \
  typename std::enable_if<(std::is_arithmetic<FirstType>::value && std::is_same<FirstType, SecondType>::value)         \
                              || (std::is_arithmetic<ToleranceType>::value && is_vector<FirstType>::value              \
                                  && is_vector<SecondType>::value                                                      \
                                  && std::is_same<ToleranceType, typename VectorAbstraction<FirstType>::R>::value      \
                                  && std::is_same<ToleranceType, typename VectorAbstraction<SecondType>::R>::value),   \
                          bool>::type                                                                                  \
  id(const FirstType& first,                                                                                           \
     const SecondType& second,                                                                                         \
     const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& rtol =                                           \
         DefaultEpsilon<ToleranceType, style>::value(),                                                                \
     const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& atol =                                           \
         DefaultEpsilon<ToleranceType, style>::value())                                                                \
  {                                                                                                                    \
    return internal::Call<FirstType, SecondType, typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type, style>::   \
        id(first, second, rtol, atol);                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  template <class FirstType, class SecondType, class ToleranceType = typename VectorAbstraction<FirstType>::R>         \
  typename std::enable_if<(std::is_arithmetic<FirstType>::value && std::is_same<FirstType, SecondType>::value)         \
                              || (std::is_arithmetic<ToleranceType>::value && is_vector<FirstType>::value              \
                                  && is_vector<SecondType>::value                                                      \
                                  && std::is_same<ToleranceType, typename VectorAbstraction<FirstType>::R>::value      \
                                  && std::is_same<ToleranceType, typename VectorAbstraction<SecondType>::R>::value),   \
                          bool>::type                                                                                  \
  id(const FirstType& first,                                                                                           \
     const SecondType& second,                                                                                         \
     const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& rtol =                                           \
         DefaultEpsilon<ToleranceType, Style::defaultStyle>::value(),                                                  \
     const typename Dune::FloatCmp::EpsilonType<ToleranceType>::Type& atol =                                           \
         DefaultEpsilon<ToleranceType, Style::defaultStyle>::value())                                                  \
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
