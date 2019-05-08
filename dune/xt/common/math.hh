// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2012 - 2018)
//   Rene Milk       (2010 - 2018)
//   Sven Kaulmann   (2013)
//   Tobias Leibner  (2014, 2017)

#ifndef DUNE_XT_COMMON_MATH_HH
#define DUNE_XT_COMMON_MATH_HH

#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <complex>

#include <dune/xt/common/disable_warnings.hh>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/format.hpp>
#include <boost/fusion/include/void.hpp>
#include <boost/geometry.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/static_assert.hpp>
#include <dune/xt/common/reenable_warnings.hh>

#include <dune/common/deprecated.hh>
#include <dune/common/promotiontraits.hh>

#include <dune/xt/common/type_traits.hh>

namespace Dune {
namespace XT {
namespace Common {


/**
 * since std::numeric_limits<T>::epsilon() is 0 for integral types
 * use this to get the minimum increment/difference for all basic types
 * (or add specializations as necessary ofc)
 **/
template<class T, bool is_integral = std::is_integral<T>::value>
struct Epsilon
{};

template<class T>
struct Epsilon<T, true>
{
  static const T value;
};

template<class T>
struct Epsilon<T, false>
{
  static const T value;
};

template<>
struct Epsilon<std::string, false>
{
  static const std::string value;
};

template<class T>
const T Epsilon<T, true>::value = T(1);
template<class T>
const T Epsilon<T, false>::value = std::numeric_limits<T>::epsilon();

namespace internal {
/**
 *  Helper struct to compute absolute values of signed and unsigned values,
 *  std::abs is only defined for signed types.
 **/
template<class T, bool isUnsigned = std::is_unsigned<T>::value>
struct AbsoluteValue
{
  static T result(const T& val)
  {
    using std::abs;
    return abs(val);
  }
};
template<class T>
struct AbsoluteValue<T, true>
{
  static T result(const T& val)
  {
    return val;
  }
};

template<class T, bool is_enum = std::is_enum<T>::value>
struct Absretval
{
  typedef T type;
};

template<class T>
struct Absretval<T, true>
{
  typedef typename underlying_type<T>::type type;
};

} // namespace internal

//! drop-in replacement for std::abs, that works for more types
template<class T>
typename internal::Absretval<T>::type abs(const T& val)
{
  typedef typename internal::Absretval<T>::type R;
  return internal::AbsoluteValue<R>::result(static_cast<R>(val));
}

//! very simple, underrun-safe for unsigned types, difference method
template<class T>
T absolute_difference(T a, T b)
{
  return (a > b) ? a - b : b - a;
}

//! a vector wrapper for continiously updating min,max,avg of some element type vector
template<class ElementType>
class MinMaxAvg
{
  static_assert(!is_complex<ElementType>::value, "complex accumulation not supported");

protected:
  typedef MinMaxAvg<ElementType> ThisType;

public:
  MinMaxAvg() {}

  template<class stl_container_type>
  MinMaxAvg(const stl_container_type& elements)
  {
    static_assert((boost::is_same<ElementType, typename stl_container_type::value_type>::value),
                  "cannot assign mismatching types");
    acc_ = std::for_each(elements.begin(), elements.end(), acc_);
  }

  std::size_t count() const
  {
    return boost::accumulators::count(acc_);
  }
  ElementType sum() const
  {
    return boost::accumulators::sum(acc_);
  }
  ElementType min() const
  {
    return boost::accumulators::min(acc_);
  }
  ElementType max() const
  {
    return boost::accumulators::max(acc_);
  }
  ElementType average() const
  {
    // for integer ElementType this just truncates from floating-point
    return ElementType(boost::accumulators::mean(acc_));
  }

  void operator()(const ElementType& el)
  {
    acc_(el);
  }

  void output(std::ostream& stream) const
  {
    stream << boost::format("min: %e\tmax: %e\tavg: %e\n") % min() % max() % average();
  }

protected:
  typedef boost::accumulators::stats<boost::accumulators::tag::max,
                                     boost::accumulators::tag::min,
                                     boost::accumulators::tag::mean,
                                     boost::accumulators::tag::count,
                                     boost::accumulators::tag::sum>
      StatsType;
  boost::accumulators::accumulator_set<ElementType, StatsType> acc_;
};

//! \return var bounded in [min, max]
template<typename T>
typename std::enable_if<!is_vector<T>::value, T>::type clamp(const T var, const T min, const T max)
{
  return (var < min) ? min : (var > max) ? max : var;
}

template<typename T>
typename std::enable_if<is_vector<T>::value, T>::type clamp(const T var, const T min, const T max)
{
  auto result = var;
  std::size_t idx = 0;
  for (auto&& element : var) {
    result[idx] = clamp(element, min[idx], max[idx]);
    ++idx;
  }
  return result;
}

/**
 * \returns: -1 iff val < 0
 *            0 iff val == 0
 *            1 iff val > 0
 */
template<typename T>
int signum(T val)
{
  return (T(0) < val) - (val < T(0));
}

/** enable us to use DXTC::numeric_limits for all types, even when no specialization is avaliable.
 * If there is one, it's used. Otherwise we default to numerical_limtis of double
 **/
template<class T, typename = void>
class numeric_limits : public std::numeric_limits<double>
{};

template<class T>
class numeric_limits<T, typename std::enable_if<std::numeric_limits<T>::is_specialized>::type>
  : public std::numeric_limits<T>
{};

//! forward to std::isnan for general types, overload for complex below
template<class T>
bool isnan(T val)
{
  using std::isnan;
  return isnan(val);
}

//! override isnan for complex here so it doesn't bleed into the std namespace
template<class T>
bool isnan(std::complex<T> val)
{
  return isnan(std::real(val)) || isnan(std::imag(val));
}

//! forward to std::isinf for general types, overload for complex below
template<class T>
bool isinf(T val)
{
  using std::isinf;
  return isinf(val);
}

//! override isinf for complex here so it doesn't bleed into the std namespace
template<class T>
bool isinf(std::complex<T> val)
{
  return isinf(std::real(val)) || isinf(std::imag(val));
}

//! calculates factorial of n
constexpr size_t factorial(size_t n)
{
  return n > 0 ? n * factorial(n - 1) : 1;
}

//! calculates complex conjugate like std::conj, but returns T instead of complex<T>
template<class T>
std::enable_if_t<std::is_arithmetic<T>::value, T> conj(T val)
{
  return val;
}

template<class T>
std::complex<T> conj(std::complex<T> val)
{
  return std::conj(val);
}

//! calculates binomial coefficient for arbitrary n
inline double binomial_coefficient(const double n, const size_t k)
{
  double ret(1);
  for (size_t ii = 1; ii <= k; ++ii)
    ret *= (n + 1 - ii) / ii;
  return ret;
}


template<class T>
T max(const T& left, const T& right)
{
  return std::max(left, right);
}

template<class L, class R>
typename PromotionTraits<L, R>::PromotedType max(const L& left, const R& right)
{
  using T = typename PromotionTraits<L, R>::PromotedType;
  return std::max(T(left), T(right));
}


template<class T>
T min(const T& left, const T& right)
{
  return std::min(left, right);
}

template<class L, class R>
typename PromotionTraits<L, R>::PromotedType min(const L& left, const R& right)
{
  using T = typename PromotionTraits<L, R>::PromotedType;
  return std::min(T(left), T(right));
}


// avoid Wfloat-equal warning
template<class FieldType>
inline bool is_zero(const FieldType& val)
{
  return std::equal_to<FieldType>()(val, FieldType(0));
}


} // namespace Common
} // namespace XT
} // namespace Dune
namespace std {


/**
 * \note \attention This is not supposed to be here! But Dune::FloatCmp uses
\code
std::abs(value)
\code
 *                  somewhere, as opposed to
\code
using std::abs;
abs(value)
\code
 *                  which would allow ADL to find the correct abs. So we have to povide std::abs for all types which
 *                  we want to use in our FloatCmp.
 */
long unsigned int abs(const long unsigned int& value);
unsigned char abs(unsigned char value);


template<int k>
Dune::bigunsignedint<k> abs(const Dune::bigunsignedint<k>& value)
{
  return value;
}

template<int k>
inline Dune::bigunsignedint<k> pow(Dune::bigunsignedint<k> /*value*/, std::uintmax_t /*n*/)
{
  DUNE_THROW(Dune::NotImplemented, "pow not implemented for bigunisgnedint");
  return Dune::bigunsignedint<k>();
}

template<int k>
inline Dune::bigunsignedint<k> sqrt(Dune::bigunsignedint<k> value)
{
  DUNE_THROW(Dune::NotImplemented, "sqrt not implemented for bigunisgnedint");
  return Dune::bigunsignedint<k>(std::sqrt(value.todouble()));
}

template<int k>
inline Dune::bigunsignedint<k> conj(Dune::bigunsignedint<k> value)
{
  return value;
}

template<int k>
inline bool isnan(Dune::bigunsignedint<k> /*value*/)
{
  return false;
}

template<int k>
inline bool isinf(Dune::bigunsignedint<k> /*value*/)
{
  return false;
}


} // namespace std


template<class T>
inline std::ostream& operator<<(std::ostream& s, const Dune::XT::Common::MinMaxAvg<T>& d)
{
  d.output(s);
  return s;
}
#endif // DUNE_XT_COMMON_MATH_HH
