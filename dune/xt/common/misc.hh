// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2009, 2012 - 2015)
//   Rene Milk       (2009 - 2015)
//   Sven Kaulmann   (2011 - 2012)
//   Tobias Leibner  (2014)

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
#include <boost/filesystem/fstream.hpp>

#include <dune/stuff/common/logging.hh>

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

//! get a non-zero initialised array
template <class T, size_t N>
std::array<T, N> make_array(const T& v)
{
  std::array<T, N> ret;
  ret.fill(v);
  return ret;
}

//! writes process environment to file
void dump_environment(boost::filesystem::ofstream& file, std::string csv_sep = ",");

/** abstraction for stdlibs w/o map.emplace
 * This isn't covering the full emplace spec, and the piecewise case potentially improper,
 * but we only need this in an Intel MIC setup with weird lib versioning.
 * Normally we'd depend on the full gcc 4.8 stack atm and do not use the insert fallback
 **/
template <typename Key, typename T, typename MapType>
std::pair<typename MapType::iterator, bool> map_emplace(MapType& map_in, Key key, T value)
{
#if HAVE_MAP_EMPLACE
  return map_in.emplace(key, value);
#else
  DSC_LOG_DEBUG_0 << "using map.insert fallback instead of emplace\n";
  return map_in.insert(typename MapType::value_type(key, value));
#endif
}

template <typename K, typename V, typename MapType>
std::pair<typename MapType::iterator, bool> map_emplace(MapType& map_in, std::piecewise_construct_t pcw, K&& keys,
                                                        V&& values)
{
#if HAVE_MAP_EMPLACE
  return map_in.emplace(pcw, keys, values);
#else
  DSC_LOG_DEBUG_0 << "using map.insert fallback instead of emplace\n";
  return map_in.insert(typename MapType::value_type(pcw, keys, values));
#endif
}

template <typename T>
struct remove_const_reference
{
  typedef typename std::remove_reference<typename std::remove_const<T>::type>::type type;
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MISC_HH
