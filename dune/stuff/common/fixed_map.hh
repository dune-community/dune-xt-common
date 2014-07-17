// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FIXED_MAP_HH
#define DUNE_STUFF_FIXED_MAP_HH

#include <array>
#include <utility>
#include <boost/array.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/assign/list_of.hpp>
#include <dune/common/exceptions.hh>
#include <dune/common/nullptr.hh>

namespace Dune {
namespace Stuff {
namespace Common {

//! custom iterator for \ref FixedMap
template <class FixedMapType>
class FixedMapIterator : public boost::iterator_facade<FixedMapIterator<FixedMapType>,
                                                       typename FixedMapType::value_type, boost::forward_traversal_tag>
{
  typedef FixedMapIterator<FixedMapType> ThisType;

public:
  FixedMapIterator()
    : index_(FixedMapType::N)
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
template <class FixedMapType>
class ConstFixedMapIterator
    : public boost::iterator_facade<ConstFixedMapIterator<FixedMapType>, const typename FixedMapType::value_type,
                                    boost::forward_traversal_tag>
{
  typedef ConstFixedMapIterator<FixedMapType> ThisType;

public:
  ConstFixedMapIterator()
    : index_(FixedMapType::N)
    , map_(nullptr)
  {
  }

  explicit ConstFixedMapIterator(const FixedMapType* const map, std::size_t i)
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
template <class key_imp, class T, std::size_t nin>
class FixedMap
{
public:
  typedef std::pair<key_imp, T> value_type;
  static const std::size_t N;

private:
  typedef boost::array<value_type, nin> MapType;

  template <class R>
  friend class FixedMapIterator;
  template <class R>
  friend class ConstFixedMapIterator;

  typedef FixedMap<key_imp, T, nin> ThisType;

public:
  typedef key_imp key_type;
  typedef T mapped_type;
  typedef FixedMapIterator<ThisType> iterator;
  typedef ConstFixedMapIterator<ThisType> const_iterator;

  FixedMap()
  {
  }
  /** inserts key-value value pairs from  initializer list
   * if list.size() > N only the first N elements are considered
   * if list.size() < N the Map is padded with default constructed elements
   */
  explicit FixedMap(const std::initializer_list<value_type>& list)
    : map_(boost::assign::list_of<value_type>(*list.begin())
               .range(list.begin() + (1 - std::min(N - list.size(), std::size_t(0))), list.end())
               .repeat(std::max(N - list.size(), std::size_t(0)), std::make_pair(key_type(), T())))
  {
  }

  explicit FixedMap(const MapType& map)
    : map_(map)
  {
  }

  std::size_t getIdx(const key_type& key) const
  {
    const auto it = std::find_if(map_.begin(), map_.end(), [&](const value_type& val) { return val.first == key; });
    if (it == map_.end())
      return N;
    return std::distance(map_.begin(), it);
  }

  const mapped_type& operator[](const key_type& key) const
  {
    const auto it = getIdx(key);
    if (it == N)
      DUNE_THROW(Dune::RangeError, "missing key in FixedMap");
    return map_[it].second;
  }

  mapped_type& operator[](const key_type& key)
  {
    const auto it = getIdx(key);
    if (it == N)
      DUNE_THROW(Dune::RangeError, "missing key in FixedMap");
    return map_[it].second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, getIdx(key));
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

template <class K, class T, std::size_t nin>
const std::size_t FixedMap<K, T, nin>::N = nin;

} // namespace Common
} // namepspace Stuff
} // namespace Dune

namespace std {
template <class key_imp, class T, std::size_t nin>
inline ostream& operator<<(ostream& out, const Dune::Stuff::Common::FixedMap<key_imp, T, nin>& map)
{
  map.print(out);
  return out;
}
}

#endif // DUNE_STUFF_FIXED_MAP_HH
