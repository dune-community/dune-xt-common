// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2017)
//   Rene Milk       (2012 - 2016, 2018)
//   Sven Kaulmann   (2012 - 2013)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_RANGES_HH
#define DUNE_XT_COMMON_RANGES_HH

#include <type_traits>

#include <boost/serialization/static_warning.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <dune/xt/common/debug.hh>
#include <dune/xt/common/math.hh>

namespace Dune {
namespace XT {
namespace Common {

//! get a vector with values in [start : increment : end)
template<class T, class sequence = std::vector<T>>
typename std::enable_if<!std::is_enum<T>::value, sequence>::type
value_range(const T start, const T end, const T increment = Epsilon<T>::value)
{
  // sadly, no overloaded version of std::abs is available for
  // unsigned long long, so we compute the absolute value of increment
  // ourselves
  const auto incrementAbs = abs(increment);
  DXT_ASSERT(incrementAbs > 0);
  sequence ret(typename sequence::size_type(((end > start) ? end - start : start - end) / incrementAbs), start);
  typename sequence::size_type i = 0;
  std::generate(std::begin(ret), std::end(ret), [&]() { return T(start + (increment * i++)); });
  return ret;
}

//! signature for enumeration Types T
template<class T, class sequence = std::vector<typename internal::Absretval<T>::type>>
typename std::enable_if<std::is_enum<T>::value, sequence>::type value_range(
    const T start,
    const T end,
    const typename internal::Absretval<T>::type increment = Epsilon<typename internal::Absretval<T>::type>::value)
{
  typedef typename internal::Absretval<T>::type R;
  return value_range(static_cast<R>(start), static_cast<R>(end), increment);
}

//! get a vector with values in [0 : Epsilon<T> : end)
template<class T, class sequence = std::vector<T>>
typename std::enable_if<!std::is_enum<T>::value, sequence>::type value_range(const T end)
{
  return value_range(T(0), end);
}

//! get a vector with values in [0 : Epsilon<T> : end)
template<class T, class sequence = std::vector<typename internal::Absretval<T>::type>>
typename std::enable_if<std::is_enum<T>::value, sequence>::type value_range(const T end)
{
  return value_range(T(0), end);
}

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_RANGES_HH
