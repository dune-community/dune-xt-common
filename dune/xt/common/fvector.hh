// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   Rene Milk       (2014 - 2016)
//   Tobias Leibner  (2014, 2016 - 2017)

#ifndef DUNE_XT_COMMON_FVECTOR_HH
#define DUNE_XT_COMMON_FVECTOR_HH

#include <initializer_list>
#include <type_traits>
#include <vector>
#include <functional>

#include <boost/functional/hash.hpp>

#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

#include <dune/xt/common/debug.hh>
#include <dune/xt/common/densevector.hh>
#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/float_cmp.hh>
#include <dune/xt/common/vector.hh>

namespace Dune {
namespace XT {
namespace Common {


template <class K, int SIZE>
class FieldVector : public Dune::FieldVector<K, SIZE>
{
  static_assert(SIZE >= 0, "Really?");

  typedef Dune::FieldVector<K, SIZE> BaseType;
  typedef FieldVector<K, SIZE> ThisType;

public:
  FieldVector(const K& kk = 0)
    : BaseType(kk)
  {
  }

  FieldVector(const size_t DXTC_DEBUG_ONLY(sz), const K& kk)
    : BaseType(kk)
  {
#ifndef NDEBUG
    if (sz != SIZE)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldVector< ..., " << SIZE << " > (of "
                                                                    << "static size) with "
                                                                    << sz
                                                                    << " elements!");
#endif // NDEBUG
  } // ... FieldVector(...)

  FieldVector(const ThisType& other) = default;

  FieldVector(const BaseType& other)
    : BaseType(other)
  {
  }

  /* FieldMatrix< K, 1, 1 > is convertible to K, which in turn is convertible to FieldVector< K, 1 >. Without the
   * following constructor, this leads to an "ambiguous constructor" error (candidates are copy constructor and
   * constructor taking a K) */
  template <class Type = K>
  FieldVector(const typename std::enable_if<SIZE == 1 && std::is_same<K, Type>::value,
                                            typename Dune::FieldMatrix<K, 1, 1>>::type& mat)
    : BaseType(mat[0][0])
  {
  }

  FieldVector(const std::vector<K>& vec)
    : BaseType(0)
  {
#ifndef NDEBUG
    if (vec.size() != SIZE)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldVector< ..., " << SIZE << " > (of "
                                                                    << "static size) from a vector of size "
                                                                    << vec.size()
                                                                    << "!");
#endif // NDEBUG
    for (size_t ii = 0; ii < SIZE; ++ii)
      this->operator[](ii) = vec[ii];
  } // FieldVector(...)

  FieldVector(std::initializer_list<K> list)
    : BaseType(0)
  {
#ifndef NDEBUG
    if (list.size() != SIZE)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldVector< ..., " << SIZE << " > (of "
                                                                    << "static size) from a list of size "
                                                                    << list.size()
                                                                    << "!");
#endif // NDEBUG
    size_t ii = 0;
    for (auto element : list)
      this->operator[](ii++) = element;
  } // FieldVector(...)

  operator std::vector<K>() const
  {
    std::vector<K> ret(SIZE);
    for (size_t ii = 0; ii < SIZE; ++ii)
      ret[ii] = this->operator[](ii);
    return ret;
  }

  operator Dune::FieldMatrix<K, 1, SIZE>() const
  {
    Dune::FieldMatrix<K, 1, SIZE> ret;
    ret[0] = *this;
    return ret;
  }

  template <int S>
  operator typename std::enable_if<(S == SIZE) && (SIZE != 1), Dune::FieldMatrix<K, S, 1>>::type() const
  {
    Dune::FieldMatrix<K, SIZE, 1> ret;
    for (size_t ii = 0; ii < SIZE; ++ii)
      ret[ii][0] = this->operator[](ii);
    return ret;
  }

  operator std::array<K, SIZE>() const
  {
    std::array<K, SIZE> ret;
    for (size_t ii = 0; ii < SIZE; ++ii)
      ret[ii] = this->operator[](ii);
    return ret;
  }

  using BaseType::operator*;

  template <int R>
  typename std::enable_if<SIZE != 1 && R == SIZE, K>::type operator*(const Dune::FieldMatrix<K, R, 1>& mat) const
  {
    K ret = 0;
    for (size_t ii = 0; ii < SIZE; ++ii)
      ret += (*this)[ii] * mat[ii][0];
    return ret;
  }

  ThisType operator*(const K& scalar) const
  {
    ThisType ret(*this);
    ret *= scalar;
    return ret;
  }

  ThisType operator/(const K& scalar) const
  {
    ThisType ret(*this);
    ret /= scalar;
    return ret;
  }
}; // class FieldVector


//! this allows to set the init value of the FieldVector at compile time
template <class K, int SIZE, K value>
class ValueInitFieldVector : public Dune::XT::Common::FieldVector<K, SIZE>
{
  typedef Dune::XT::Common::FieldVector<K, SIZE> BaseType;

public:
  ValueInitFieldVector()
    : BaseType(value)
  {
  }
}; // class ValueInitFieldVector


//! struct to be used as comparison function e.g. in a std::map<FieldVector<...>, ..., FieldVectorLess>
struct FieldVectorLess
{
  template <class FieldType, int dimDomain>
  bool operator()(const Dune::FieldVector<FieldType, dimDomain>& a,
                  const Dune::FieldVector<FieldType, dimDomain>& b) const
  {
    for (size_t dd = 0; dd < dimDomain; ++dd) {
      if (XT::Common::FloatCmp::lt(a[dd], b[dd]))
        return true;
      else if (XT::Common::FloatCmp::gt(a[dd], b[dd]))
        return false;
    }
    return false;
  }
};


//! Specialization of VectorAbstraction for Dune::XT::Common::FieldVector
template <class K, int SIZE>
struct VectorAbstraction<Dune::XT::Common::FieldVector<K, SIZE>>
    : public internal::VectorAbstractionBase<Dune::XT::Common::FieldVector<K, SIZE>, K>,
      public internal::HasSubscriptOperatorForVectorAbstraction<Dune::XT::Common::FieldVector<K, SIZE>,
                                                                typename Dune::FieldTraits<K>::field_type>,
      public internal::IsSizeTransferableForVectorAbstraction<int>::Base<Dune::XT::Common::FieldVector, SIZE>
{
  static const bool has_static_size = true;
  static const size_t static_size = SIZE;

  static inline Dune::XT::Common::FieldVector<K, SIZE> create(const size_t sz)
  {
    return Dune::XT::Common::FieldVector<K, SIZE>(sz);
  }

  static inline Dune::XT::Common::FieldVector<K, SIZE> create(const size_t sz, const K& val)
  {
    return Dune::XT::Common::FieldVector<K, SIZE>(sz, val);
  }
};


} // namespace Common
} // namespace XT


template <class K, int SIZE>
typename std::enable_if<SIZE != 1, K>::type operator*(const Dune::FieldVector<K, SIZE>& vec,
                                                      const Dune::FieldMatrix<K, SIZE, 1>& mat)
{
  K ret = 0;
  for (size_t ii = 0; ii < SIZE; ++ii)
    ret += vec[ii] * mat[ii][0];
  return ret;
}


} // namespace Dune
namespace std {


/**
 * \sa http://en.cppreference.com/w/cpp/utility/hash
 * \sa http://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 */
template <class K, int SIZE>
struct hash<Dune::FieldVector<K, SIZE>>
{
  typedef Dune::FieldVector<K, SIZE> argument_type;
  typedef std::size_t result_type;

  result_type operator()(argument_type const& s) const noexcept
  {
    std::size_t seed = 0;
    for (size_t ii = 0; ii < s.size(); ++ii)
      boost::hash_combine(seed, s[ii]);
    return seed;
  }
}; // struct hash<Dune::FieldVector<...>>


/**
 * \sa http://en.cppreference.com/w/cpp/utility/hash
 * \sa http://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
 */
template <class K, int SIZE>
struct hash<Dune::XT::Common::FieldVector<K, SIZE>>
{
  typedef Dune::XT::Common::FieldVector<K, SIZE> argument_type;
  typedef std::size_t result_type;

  result_type operator()(argument_type const& s) const noexcept
  {
    std::size_t seed = 0;
    for (size_t ii = 0; ii < s.size(); ++ii)
      boost::hash_combine(seed, s[ii]);
    return seed;
  }
}; // struct hash<Dune::XT::Common::FieldVector<...>>


} // namespace std

#endif // DUNE_XT_COMMON_FVECTOR_HH
