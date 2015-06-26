// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_INTERNAL_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_INTERNAL_HH

#include <type_traits>
#include <complex>

#include <dune/common/float_cmp.hh>

#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/float_cmp_style.hh>
//#include <dune/stuff/common/vector.hh>

namespace Dune {
namespace FloatCmp {
template <class T>
struct EpsilonType<std::complex<T>>
{
  //! The epsilon type corresponding to value type std::complex<T>
  typedef EpsilonType<T> Type;
};
}
namespace Stuff {
namespace Common {


// forward, include is below
template <class VecType>
struct VectorAbstraction;

template <class VecType>
struct is_vector;


namespace FloatCmp {
namespace internal {


/**
 * Taken from http://docs.pymor.org/en/latest/_modules/pymor/tools/floatcmp.html#float_cmp,
 *            https://github.com/numpy/numpy/blob/v1.9.1/numpy/core/numeric.py#L2238
 */
template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type float_cmp_eq(const T& xx, const T& yy, const T& rtol,
                                                                               const T& atol)
{
  return std::abs(xx - yy) <= atol + std::abs(yy) * rtol;
}


template <class T>
typename std::enable_if<is_complex<T>::value, bool>::type
float_cmp_eq(const T& xx, const T& yy, const typename T::value_type& rtol, const typename T::value_type& atol)
{
  return (float_cmp_eq(std::real(xx), std::real(yy), rtol, atol)
          && float_cmp_eq(std::imag(xx), std::imag(yy), rtol, atol));
}


template <class XType, class YType, class TolType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value && std::is_arithmetic<TolType>::value
                            && std::is_same<typename VectorAbstraction<XType>::R, TolType>::value
                            && std::is_same<typename VectorAbstraction<YType>::R, TolType>::value,
                        bool>::type
float_cmp_eq(const XType& xx, const YType& yy, const TolType& rtol, const TolType& atol)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!float_cmp_eq(xx[ii], yy[ii], rtol, atol))
      return false;
  return true;
} // ... float_cmp(...)

template <Dune::FloatCmp::CmpStyle style, class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type dune_float_cmp_eq(const T& xx, const T& yy,
                                                                                    const T& eps)
{
  return Dune::FloatCmp::eq<T, style>(xx, yy, eps);
}

template <Dune::FloatCmp::CmpStyle style, class T>
bool dune_float_cmp_eq(const std::complex<T>& xx, const std::complex<T>& yy, const T& eps)
{
  return Dune::FloatCmp::eq<T, style>(xx.real(), yy.real(), eps)
         && Dune::FloatCmp::eq<T, style>(xx.imag(), yy.imag(), eps);
}

template <Dune::FloatCmp::CmpStyle style, class XType, class YType, class EpsType>
typename std::enable_if<is_vector<XType>::value && is_vector<YType>::value && std::is_arithmetic<EpsType>::value
                            && std::is_same<typename VectorAbstraction<XType>::R, EpsType>::value
                            && std::is_same<typename VectorAbstraction<YType>::R, EpsType>::value,
                        bool>::type
dune_float_cmp_eq(const XType& xx, const YType& yy, const EpsType& eps)
{
  const auto sz = xx.size();
  if (yy.size() != sz)
    return false;
  for (size_t ii = 0; ii < sz; ++ii)
    if (!dune_float_cmp_eq<style>(xx[ii], yy[ii], eps))
      return false;
  return true;
} // ... dune_float_cmp(...)

template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type cmp_gt(const T& xx, const T& yy)
{
  return std::greater<T>()(xx, yy);
}

template <class T>
bool cmp_gt(const std::complex<T>& xx, const std::complex<T>& yy)
{
  T x = std::abs(xx);
  T y = std::abs(yy);
  static_assert(std::is_arithmetic<T>::value, "");
  return std::greater<double>()(x, y);
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
    if (!(cmp_gt(xx[ii], yy[ii])))
      return false;
  return true;
} // ... cmp_gt(...)


template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type cmp_lt(const T& xx, const T& yy)
{
  return std::less<T>()(xx, yy);
}

template <class T>
bool cmp_lt(const std::complex<T>& xx, const std::complex<T>& yy)
{
  return std::less<T>()(std::abs(xx), std::abs(yy));
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
    if (!(cmp_lt(xx[ii], yy[ii])))
      return false;
  return true;
} // ... cmp_lt(...)


template <class FirstType, class SecondType, class ToleranceType, Style style>
struct Call
{
  static bool eq(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol);
  }

  static bool ne(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol);
  }

  static bool gt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol) && cmp_gt(first, second);
  }

  static bool lt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return !dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol) && cmp_lt(first, second);
  }

  static bool ge(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol) || cmp_gt(first, second);
  }

  static bool le(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& /**/)
  {
    return dune_float_cmp_eq<internal::ConvertStyle<style>::value>(first, second, rtol) || cmp_lt(first, second);
  }
};

template <class FirstType, class SecondType, class ToleranceType>
struct Call<FirstType, SecondType, ToleranceType, Style::numpy>
{
  static bool eq(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp_eq(first, second, rtol, atol);
  }

  static bool ne(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp_eq(first, second, rtol, atol);
  }

  static bool gt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp_eq(first, second, rtol, atol) && cmp_gt(first, second);
  }

  static bool lt(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return !float_cmp_eq(first, second, rtol, atol) && cmp_lt(first, second);
  }

  static bool ge(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp_eq(first, second, rtol, atol) || cmp_gt(first, second);
  }

  static bool le(const FirstType& first, const SecondType& second, const ToleranceType& rtol, const ToleranceType& atol)
  {
    return float_cmp_eq(first, second, rtol, atol) || cmp_lt(first, second);
  }
};

template <class FirstType, class SecondType, class ToleranceType = typename VectorAbstraction<FirstType>::R>
struct cmp_type_check
{
  static constexpr bool is_ok_scalar = (std::is_arithmetic<FirstType>::value || is_complex<FirstType>::value)
                                       && std::is_same<FirstType, SecondType>::value;
  static constexpr bool is_ok_vector = std::is_arithmetic<ToleranceType>::value && is_vector<FirstType>::value
                                       && is_vector<SecondType>::value
                                       && std::is_same<ToleranceType, typename VectorAbstraction<FirstType>::R>::value
                                       && std::is_same<ToleranceType, typename VectorAbstraction<SecondType>::R>::value;
  static constexpr bool value = is_ok_scalar || is_ok_vector;
};

} // namespace internal
} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#include <dune/stuff/common/vector.hh>

#endif // DUNE_STUFF_COMMON_FLOAT_CMP_INTERNAL_HH
