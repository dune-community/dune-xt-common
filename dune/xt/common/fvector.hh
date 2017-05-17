// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   Rene Milk       (2014 - 2016)
//   Tobias Leibner  (2014, 2016)

#ifndef DUNE_XT_COMMON_FVECTOR_HH
#define DUNE_XT_COMMON_FVECTOR_HH

#include <initializer_list>
#include <vector>

#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/debug.hh>
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
  FieldVector(const K kk = K(0))
    : BaseType(kk)
  {
  }

  FieldVector(const size_t DXTC_DEBUG_ONLY(sz), const K kk)
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
    : BaseType(K(0))
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
    : BaseType(K(0))
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

  ThisType& operator=(const BaseType& other)
  {
    BaseType::operator=(other);
    return *this;
  }

  operator std::vector<K>() const
  {
    std::vector<K> ret(SIZE);
    for (size_t ii = 0; ii < SIZE; ++ii)
      ret[ii] = this->operator[](ii);
    return ret;
  }

  using BaseType::operator*;

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


  ThisType operator+(const BaseType& other) const
  {
    ThisType ret(*this);
    ret += other;
    return ret;
  }

  ThisType operator-(const BaseType& other) const
  {
    ThisType ret(*this);
    ret -= other;
    return ret;
  }

}; // class FieldVector

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

//! Specialization of VectorAbstraction for Dune::XT::Common::FieldVector
template <class K, int SIZE>
struct VectorAbstraction<Dune::XT::Common::FieldVector<K, SIZE>>
{
  typedef Dune::XT::Common::FieldVector<K, SIZE> VectorType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const bool is_vector = true;

  static const bool has_static_size = true;

  static const size_t static_size = SIZE;

  static inline VectorType create(const size_t sz)
  {
    return VectorType(sz, ScalarType(0));
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    return VectorType(sz, val);
  }
};

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_FVECTOR_HH
