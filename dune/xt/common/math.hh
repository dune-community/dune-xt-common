// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2012 - 2015)
//   Rene Milk       (2010 - 2015)
//   Sven Kaulmann   (2013)
//   Tobias Leibner  (2014)

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
#include <boost/static_assert.hpp>
#include <boost/fusion/include/void.hpp>
#include <boost/format.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <dune/xt/common/reenable_warnings.hh>

#include <dune/xt/common/type_utils.hh>
#include <dune/common/deprecated.hh>

namespace Dune {
namespace XT {
namespace Common {

/**
 * since std::numeric_limits<T>::epsilon() is 0 for integral types
 * use this to get the minimum increment/difference for all basic types
 * (or add specializations as necessary ofc)
 **/
template <class T, bool is_integral = std::is_integral<T>::value>
struct Epsilon
{
};

template <class T>
struct Epsilon<T, true>
{
  static const T value;
};

template <class T>
struct Epsilon<T, false>
{
  static const T value;
};

template <>
struct Epsilon<std::string, false>
{
  static const std::string value;
};

template <class T>
const T Epsilon<T, true>::value = 1;
template <class T>
const T Epsilon<T, false>::value = std::numeric_limits<T>::epsilon();

/**
 *  Helper struct to compute absolute values of signed and unsigned values,
 *  std::abs is only defined for signed types.
 **/
template <class T, bool isUnsigned = std::is_unsigned<T>::value>
struct absoluteValue
{
  static T result(const T& val)
  {
    return std::abs(val);
  }
};
template <class T>
struct absoluteValue<T, true>
{
  static T result(const T& val)
  {
    return val;
  }
};

template <class T, bool is_enum = std::is_enum<T>::value>
struct absretval
{
  typedef T type;
};

template <class T>
struct absretval<T, true>
{
  typedef typename underlying_type<T>::type type;
};

template <class T>
typename absretval<T>::type abs(const T& val)
{
  typedef typename absretval<T>::type R;
  return absoluteValue<R>::result(static_cast<R>(val));
}

//! a vector wrapper for continiously updating min,max,avg of some element type vector
template <class ElementType>
class MinMaxAvg
{
  static_assert(!is_complex<ElementType>::value, "complex accumulation not supported");

protected:
  typedef MinMaxAvg<ElementType> ThisType;

public:
  MinMaxAvg()
  {
  }

  template <class stl_container_type>
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

  void output(std::ostream& stream)
  {
    stream << boost::format("min: %e\tmax: %e\tavg: %e\n") % min() % max() % average();
  }

protected:
  typedef boost::accumulators::stats<boost::accumulators::tag::max, boost::accumulators::tag::min,
                                     boost::accumulators::tag::mean, boost::accumulators::tag::count,
                                     boost::accumulators::tag::sum> StatsType;
  boost::accumulators::accumulator_set<ElementType, StatsType> acc_;
};

//! \return var bounded in [min, max]
template <typename T>
T clamp(const T var, const T min, const T max)
{
  return (var < min) ? min : (var > max) ? max : var;
}

/**
 * \returns: -1 iff val < 0
 *            0 iff val == 0
 *            1 iff val > 0
 */
template <typename T>
int signum(T val)
{
  return (T(0) < val) - (val < T(0));
}

/** enable us to use DSC::numeric_limits for all types, even when no specialization is avaliable.
 * If there is one, it's used. Otherwise we default to numerical_limtis of double
 **/
template <class T, typename = void>
class numeric_limits : public std::numeric_limits<double>
{
};

template <class T>
class numeric_limits<T, typename std::enable_if<std::numeric_limits<T>::is_specialized>::type>
    : public std::numeric_limits<T>
{
};

//! forward to std::isnan for general types, overload for complex below
template <class T>
bool isnan(T val)
{
  return std::isnan(val);
}

//! override isnan for complex here so it doesn't bleed into the std namespace
template <class T>
bool isnan(std::complex<T> val)
{
  return isnan(std::real(val)) || isnan(std::imag(val));
}

//! forward to std::isinf for general types, overload for complex below
template <class T>
bool isinf(T val)
{
  return std::isinf(val);
}

//! override isinf for complex here so it doesn't bleed into the std namespace
template <class T>
bool isinf(std::complex<T> val)
{
  return isinf(std::real(val)) || isinf(std::imag(val));
}

} // namespace Common
} // namespace XT
} // namespace Dune

namespace std {
}
#endif // DUNE_XT_COMMON_MATH_HH
