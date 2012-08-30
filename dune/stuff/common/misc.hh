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
#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#else
#include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG
#include <dune/common/version.hh>
#include <dune/common/array.hh>
#include <dune/common/static_assert.hh>
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
#include <cstdio>
#include <initializer_list>
#include <dune/common/deprecated.hh>
#include <dune/common/exceptions.hh>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace Dune {
namespace Stuff {
namespace Common {
namespace Misc {

//! element-index-in-container search
template <class StlSequence>
inline int getIdx(const StlSequence& ct, const typename StlSequence::value_type& val)
{
  typename StlSequence::const_iterator result = std::find(ct.begin(), ct.end(), val);
  if (result == ct.end())
    return -1;

  return std::distance(ct.begin(), result);
} // getIdx

/** \brief stupid timing helper
   * \deprecated in favor of boost automatic_timer (in boost >= 1.48)
   **/
struct TimeGuard
{
  const time_t cur_time;
  DUNE_DEPRECATED_MSG("use boost automatic timer instead")
  TimeGuard()
    : cur_time(time(NULL))
  {
  }
  ~TimeGuard()
  {
    time_t delta = time(NULL) - cur_time;

    std::cout << ctime(&delta) << std::endl;
  }
};

//! \todo seems borked, resutls in gigantic amount of compile errors?!
template <class StlSequence, class T>
void fill_entirely(StlSequence& c, const T& value)
{
  std::fill(c.begin(), c.end(), value);
}

/** this allows subscription indices to wrap around
   * \example N=4: wraparound_array[4] == wraparound_array[0] && wraparound_array[-1] == wraparound_array[3]
   **/
template <class T, int N>
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
    std::size_t real_index = i;

    if (i < 0)
      real_index = static_cast<size_t>(N - (((i * -1) % N) + 1));
    return BaseType::operator[](real_index);
  } // []

  typename BaseType::const_reference operator[](std::size_t i) const
  {
    return BaseType::operator[](i % N);
  }

  typename BaseType::const_reference operator[](int i) const
  {
    std::size_t real_index = i;

    if (i < 0)
      real_index = static_cast<size_t>(N - (((i * -1) % N) + 1));
    return BaseType::operator[](real_index);
  } // []
};

//! type safe (this will not compile for degraded-to-pointer arrays) way of getting array length
template <class T, size_t N>
size_t arrayLength(T(&/*array*/)[N])
{
  return N;
}

template <class FixedMapType>
class FixedMapIterator : public boost::iterator_facade<FixedMapIterator<FixedMapType>,
                                                       typename FixedMapType::value_type, boost::forward_traversal_tag>
{
  typedef FixedMapIterator<FixedMapType> ThisType;

  static const std::size_t N;

public:
  FixedMapIterator()
    : index_(N)
    , map_(nullptr)
  {
  }

  explicit FixedMapIterator(FixedMapType* map, std::size_t i)
    : index_(i)
    , map_(map)
  {
  }

private:
  friend class boost::iterator_core_access;

  void increment()
  {
    index_++;
  }

  bool equal(ThisType const& other) const
  {
    return this->map_ == other.map_;
  }

  typename FixedMapType::value_type& dereference() const
  {
    return map_->map_[index_];
  }

  std::size_t index_;
  FixedMapType* map_;
};

template <class Y>
const std::size_t FixedMapIterator<Y>::N = Y::N;

//! a std::map like container that prevent map size change
template <class key_imp, class T, std::size_t nin>
class FixedMap
{
public:
  typedef std::pair<key_imp, T> value_type;
  static const std::size_t N;

private:
  typedef std::array<value_type, nin> MapType;

  template <class R>
  friend class FixedMapIterator;
  typedef FixedMap<key_imp, T, nin> ThisType;

public:
  typedef key_imp key_type;
  typedef T mapped_type;
  typedef FixedMapIterator<ThisType> iterator;
  typedef FixedMapIterator<const ThisType> const_iterator;

  FixedMap()
  {
  }
  FixedMap(const MapType& map)
  {
    map_ = map;
  }

  std::size_t getIdx(const key_type& key) const
  {
    //    auto it =
    assert(false);
    return -1;
  }

  const mapped_type& operator[](const key_type& key) const
  {
    const auto it = getIdx(key);
    if (it != -1)
      DUNE_THROW(Dune::RangeError, "missing key in FixedMap");
    return map_[it].second;
  }

  mapped_type& operator[](const key_type& key)
  {
    const auto it = getIdx(key);
    if (it != -1)
      DUNE_THROW(Dune::RangeError, "missing key in FixedMap");
    return map_[it].second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(*this, getIdx(key));
  }

  iterator find(const key_type& key)
  {
    return iterator(this, getIdx(key));
  }

  iterator begin()
  {
    return iterator(this, 0);
  }

  iterator end()
  {
    return iterator(this, N);
  }

  const_iterator begin() const
  {
    return const_iterator(this, 0);
  }

  const_iterator end() const
  {
    return const_iterator(this, N);
  }

private:
  MapType map_;
};

template <class K, class T, std::size_t nin>
const std::size_t FixedMap<K, T, nin>::N = nin;

} // namespace Misc
} // namespace Common
} // namepspace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_MISC_HH

/** Copyright (c) 2012, Rene Milk    , Sven Kaulmann
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
