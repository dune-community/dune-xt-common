// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2009, 2012 - 2014, 2016 - 2017)
//   Rene Milk       (2009 - 2018)
//   Sven Kaulmann   (2011 - 2012)
//   Tobias Leibner  (2014, 2016)

#ifndef DUNE_XT_COMMON_FIXED_MAP_HH
#define DUNE_XT_COMMON_FIXED_MAP_HH

#include <array>
#include <utility>
#include <boost/array.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/assign/list_of.hpp>
#include <dune/common/exceptions.hh>

namespace Dune {
namespace XT {
namespace Common {

//! custom iterator for \ref FixedMap
template<class FixedMapType>
class FixedMapIterator
  : public boost::
        iterator_facade<FixedMapIterator<FixedMapType>, typename FixedMapType::value_type, boost::forward_traversal_tag>
{
  typedef FixedMapIterator<FixedMapType> ThisType;

public:
  FixedMapIterator()
    : index_(FixedMapType::N)
    , map_(nullptr)
  {}

  explicit FixedMapIterator(FixedMapType* map, std::size_t i)
    : index_(i)
    , map_(map)
  {}

private:
  friend class boost::iterator_core_access;

  void increment()
  {
    index_++;
  }

  bool equal(ThisType const& other) const
  {
    return this->map_ && (index_ == other.index_) && (this->map_ == other.map_);
  }

  typename FixedMapType::value_type& dereference() const
  {
    return map_->map_[index_];
  }

  std::size_t index_;
  FixedMapType* const map_;
};

//! custom const iterator for \ref FixedMap
template<class FixedMapType>
class ConstFixedMapIterator
  : public boost::iterator_facade<ConstFixedMapIterator<FixedMapType>,
                                  const typename FixedMapType::value_type,
                                  boost::forward_traversal_tag>
{
  typedef ConstFixedMapIterator<FixedMapType> ThisType;

public:
  ConstFixedMapIterator()
    : index_(FixedMapType::N)
    , map_(nullptr)
  {}

  explicit ConstFixedMapIterator(const FixedMapType* const map, std::size_t i)
    : index_(i)
    , map_(map)
  {}

private:
  friend class boost::iterator_core_access;

  void increment()
  {
    index_++;
  }

  bool equal(ThisType const& other) const
  {
    return this->map_ && (index_ == other.index_) && (this->map_ == other.map_);
  }

  const typename FixedMapType::value_type& dereference() const
  {
    return map_->map_[index_];
  }

  std::size_t index_;
  const FixedMapType* const map_;
};

//! a std::map like container that prevents map size change
template<class key_imp, class T, std::size_t nin>
class FixedMap
{
public:
  typedef std::pair<key_imp, T> value_type;
  static const std::size_t N;

private:
  typedef boost::array<value_type, nin> MapType;

  template<class R>
  friend class FixedMapIterator;
  template<class R>
  friend class ConstFixedMapIterator;

  typedef FixedMap<key_imp, T, nin> ThisType;

  template<class K> // for sfinae to work this needs to be a template although the type is already fixed
  typename std::enable_if<std::is_convertible<K, std::string>::value, std::string>::type
  range_error_message(K key) const
  {
    std::stringstream ss;
    ss << "missing key '" << key << "' in FixedMap!";
    return ss.str();
  }

  template<class K>
  typename std::enable_if<std::is_convertible<K, int>::value, std::string>::type range_error_message(K key) const
  {
    std::stringstream ss;
    ss << "missing key (converted to int)'" << int(key) << "' in FixedMap!";
    return ss.str();
  }

  template<class K>
  typename std::enable_if<!(std::is_convertible<K, int>::value || std::is_convertible<K, std::string>::value),
                          std::string>::type range_error_message(K /*key*/) const
  {
    return "missing key is not printable";
  }

public:
  typedef key_imp key_type;
  typedef T mapped_type;
  typedef FixedMapIterator<ThisType> iterator;
  typedef ConstFixedMapIterator<ThisType> const_iterator;

  FixedMap() {}
  /** inserts key-value value pairs from  initializer list
   * if list.size() > N only the first N elements are considered
   * if list.size() < N the Map is padded with default constructed elements
   */
  FixedMap(const std::initializer_list<value_type>& list)
    : map_(boost::assign::list_of<value_type>(*list.begin())
               .range(list.begin() + 1, list.end() - (N > list.size() ? size_t(0) : (list.size() - N)))
               .repeat(N > list.size() ? N - list.size() : size_t(0), std::make_pair(key_type(), T())))
  {}

  FixedMap(const MapType& map)
    : map_(map)
  {}

  std::size_t get_idx(const key_type& key) const
  {
    const auto it = std::find_if(map_.begin(), map_.end(), [&](const value_type& val) { return val.first == key; });
    if (it == map_.end())
      return N;
    return std::distance(map_.begin(), it);
  }

  const mapped_type& operator[](const key_type& key) const
  {
    const auto it = get_idx(key);
    if (it == N)
      DUNE_THROW(RangeError, range_error_message(key));
    return map_[it].second;
  }

  mapped_type& operator[](const key_type& key)
  {
    const auto it = get_idx(key);
    if (it == N)
      DUNE_THROW(RangeError, range_error_message(key));
    return map_[it].second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, get_idx(key));
  }

  iterator find(const key_type& key)
  {
    return iterator(this, get_idx(key));
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

  std::size_t size() const
  {
    return N;
  }

  void print(std::ostream& out) const
  {
    out << "{";
    for (const auto& vl : map_) {
      out << vl.first << ": " << vl.second << ", ";
    }
    out << "}";
  }

private:
  MapType map_;
};

template<class K, class T, std::size_t nin>
const std::size_t FixedMap<K, T, nin>::N = nin;

} // namespace Common
} // namespace XT
} // namespace Dune

namespace std {
template<class key_imp, class T, std::size_t nin>
inline ostream& operator<<(ostream& out, const Dune::XT::Common::FixedMap<key_imp, T, nin>& map)
{
  map.print(out);
  return out;
}
}

#endif // DUNE_XT_COMMON_FIXED_MAP_HH
