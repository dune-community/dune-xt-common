// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

/**
   *  \file   stuff.hh
   *  \brief  contains some stuff
   **/
#ifndef DUNE_STUFF_COMMON_MISC_HH
#define DUNE_STUFF_COMMON_MISC_HH

#include <cstring>
#include <map>
#include <assert.h>
#include <algorithm>
#include <dune/common/version.hh>
#include <dune/common/array.hh>
#include <dune/common/deprecated.hh>
#include <dune/common/exceptions.hh>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cerrno>
#include <limits>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>

#include <dune/stuff/common/disable_warnings.hh>
#include <boost/algorithm/string.hpp>
#include <dune/stuff/common/reenable_warnings.hh>

namespace Dune {
namespace Stuff {
namespace Common {

//! element-index-in-container search
template <class StlSequence>
inline int getIdx(const StlSequence& ct, const typename StlSequence::value_type& val)
{
  const auto result = std::find(ct.begin(), ct.end(), val);
  if (result == ct.end())
    return -1;
  return std::distance(ct.begin(), result);
} // getIdx


/** this allows subscription indices to wrap around
   * \example N=4: wraparound_array[4] == wraparound_array[0] && wraparound_array[-1] == wraparound_array[3]
   **/
template <class T, size_t N>
struct wraparound_array : public Dune::array<T, N>
{
  typedef Dune::array<T, N> BaseType;
  wraparound_array()
  {
    for (size_t i = 0; i < N; ++i)
      this->operator[](i) = T();
  }

  wraparound_array(const BaseType other)
  {
    for (size_t i = 0; i < N; ++i)
      this->operator[](i) = other[i];
  }

  typename BaseType::reference operator[](std::size_t i)
  {
    return BaseType::operator[](i % N);
  }

  typename BaseType::reference operator[](int i)
  {
    const std::size_t real_index = i < 0 ? static_cast<std::size_t>(N - (((i * -1) % N) + 1)) : std::size_t(i);
    return BaseType::operator[](real_index);
  } // []

  typename BaseType::const_reference operator[](std::size_t i) const
  {
    return BaseType::operator[](i % N);
  }

  typename BaseType::const_reference operator[](int i) const
  {
    const std::size_t real_index = i < 0 ? static_cast<std::size_t>(N - (((i * -1) % N) + 1)) : std::size_t(i);
    return BaseType::operator[](real_index);
  } // []
};

//! type safe (this will not compile for degraded-to-pointer arrays) way of getting array length
template <class T, size_t N>
size_t arrayLength(T(&/*array*/)[N])
{
  return N;
}


} // namespace Common
} // namepspace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MISC_HH
