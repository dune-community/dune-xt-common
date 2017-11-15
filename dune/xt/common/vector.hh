// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Barbara Verfürth (2015)
//   Felix Schindler  (2012 - 2017)
//   Rene Milk        (2012 - 2016)
//   Sven Kaulmann    (2013)
//   Tobias Leibner   (2014 - 2017)

#ifndef DUNE_XT_COMMON_VECTOR_HH
#define DUNE_XT_COMMON_VECTOR_HH

#include <vector>
#include <ostream>

#include <dune/common/densevector.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>
#include <dune/common/ftraits.hh>
#include <dune/common/streamoperators.hh> // for operator<<(stream, std::array<>)

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/type_traits.hh>

namespace Dune {
namespace XT {
namespace Common {


// forward
template <class VecType>
struct VectorAbstraction;


//! logically and structurally this belongs in type_utils.hh, but the dependent implementation prohibits that
template <class VectorType>
struct is_vector
{
  static const bool value = VectorAbstraction<VectorType>::is_vector;
};


namespace internal {


/**
 * \brief A base class to derive from when specializing \sa VectorAbstraction.
 */
template <class Vector, class Field>
struct VectorAbstractionBase
{
  using VectorType = Vector;
  using ScalarType = typename Dune::FieldTraits<Field>::field_type;
  using RealType = typename Dune::FieldTraits<Field>::real_type;
  using S = ScalarType;
  using R = RealType;

  static const constexpr bool is_vector = true;
  static const constexpr bool has_static_size = false;
  static const constexpr size_t static_size = std::numeric_limits<size_t>::max();
  static const constexpr bool has_ostream = false;
}; // struct VectorAbstractionBase


/**
 * \brief A base class to derive from when specializing \sa VectorAbstraction.
 */
template <class VectorType, class ScalarType>
struct HasSubscriptOperatorForVectorAbstraction
{
  static inline ScalarType get_entry(const VectorType& vector, const size_t ii)
  {
    assert(ii < vector.size());
    return vector[ii];
  }

  static inline void set_entry(VectorType& vector, const size_t ii, const ScalarType& val)
  {
    assert(ii < vector.size());
    vector[ii] = val;
  }
};


} // namespace internal


/**
 * \brief Traits to uniformly handle dense (mathematical) vectors.
 *
 *        If you want your vector class to benefit from the functionality in this header you have to manually
 *        specify a specialization of this class in your code with is_vector defined to true and the appropriate
 *        static methods implemented and members defined (see the specializations below).
 * \sa internal::VectorAbstractionBase
 * \sa internal::HasSubscriptOperatorForVectorAbstraction
 */
template <class VecType>
struct VectorAbstraction
{
  typedef VecType VectorType;
  typedef typename Dune::FieldTraits<VecType>::field_type ScalarType;
  typedef typename Dune::FieldTraits<VecType>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const constexpr bool is_vector = false;
  static const constexpr bool has_static_size = false;
  static const constexpr size_t static_size = std::numeric_limits<size_t>::max();
  static const constexpr bool has_ostream = true;

  static inline /*VectorType*/ void create(const size_t /*sz*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }

  static inline /*VectorType*/ void create(const size_t /*sz*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }

  static inline /*ScalarType*/ void get_entry(const VectorType& /*vec*/, const size_t /*ii*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }

  static inline void set_entry(VectorType& /*vec*/, size_t /*ii*/, ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }
};


template <class T>
struct VectorAbstraction<std::vector<T>>
    : public internal::VectorAbstractionBase<std::vector<T>, T>,
      public internal::HasSubscriptOperatorForVectorAbstraction<std::vector<T>,
                                                                typename Dune::FieldTraits<T>::field_type>
{
  static inline std::vector<T> create(const size_t sz)
  {
    return std::vector<T>(sz);
  }

  static inline std::vector<T> create(const size_t sz, const T& val)
  {
    return std::vector<T>(sz, val);
  }
};

template <class K, size_t SIZE>
struct VectorAbstraction<std::array<K, SIZE>>
    : public internal::VectorAbstractionBase<std::array<K, SIZE>, K>,
      public internal::HasSubscriptOperatorForVectorAbstraction<std::array<K, SIZE>,
                                                                typename Dune::FieldTraits<K>::field_type>
{
  static const constexpr bool has_static_size = true;
  static const constexpr size_t static_size = SIZE;
  static const constexpr bool has_ostream = true; // provided in dune/common/streamoperators.hh

  static inline std::array<K, SIZE> create(const size_t sz)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "requested size: " << sz << "\n   actual size: " << int(SIZE));
    return std::array<K, SIZE>();
  }

  static inline std::array<K, SIZE> create(const size_t sz, const K& val)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "requested size: " << sz << "\n   actual size: " << int(SIZE));
    std::array<K, SIZE> array;
    array.fill(val);
    return array;
  }
};

template <class K>
struct VectorAbstraction<Dune::DynamicVector<K>>
    : public internal::VectorAbstractionBase<Dune::DynamicVector<K>, K>,
      public internal::HasSubscriptOperatorForVectorAbstraction<Dune::DynamicVector<K>,
                                                                typename Dune::FieldTraits<K>::field_type>
{
  static inline Dune::DynamicVector<K> create(const size_t sz)
  {
    return Dune::DynamicVector<K>(sz);
  }

  static inline Dune::DynamicVector<K> create(const size_t sz, const K& val)
  {
    return Dune::DynamicVector<K>(sz, val);
  }
};

template <class K, int SIZE>
struct VectorAbstraction<Dune::FieldVector<K, SIZE>>
    : public internal::VectorAbstractionBase<Dune::FieldVector<K, SIZE>, K>,
      public internal::HasSubscriptOperatorForVectorAbstraction<Dune::FieldVector<K, SIZE>,
                                                                typename Dune::FieldTraits<K>::field_type>
{
  static const constexpr bool has_static_size = true;
  static const constexpr size_t static_size = SIZE;

  static inline Dune::FieldVector<K, SIZE> create(const size_t sz)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "requested size: " << sz << "\n   actual size: " << int(SIZE));
    return Dune::FieldVector<K, SIZE>();
  }

  static inline Dune::FieldVector<K, SIZE> create(const size_t sz, const K& val)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "requested size: " << sz << "\n   actual size: " << int(SIZE));
    return Dune::FieldVector<K, SIZE>(val);
  }
};

template <class V>
struct VectorAbstraction<Dune::DenseVector<V>> : public VectorAbstraction<typename Dune::DenseVector<V>::derived_type>
{
};


template <class VectorType>
typename std::enable_if<is_vector<VectorType>::value, VectorType>::type
create(const size_t sz,
       const typename VectorAbstraction<VectorType>::S& val = typename VectorAbstraction<VectorType>::S(0))
{
  return VectorAbstraction<VectorType>::create(sz, val);
}


template <class TargetVectorType, class SourceVectorType>
typename std::enable_if<is_vector<TargetVectorType>::value && is_vector<SourceVectorType>::value,
                        TargetVectorType>::type
zeros_like(const SourceVectorType& source)
{
  return create<TargetVectorType>(source.size(), 0);
}


template <class VectorType>
typename std::enable_if<is_vector<VectorType>::value, VectorType>::type zeros_like(const VectorType& source)
{
  return zeros_like<VectorType, VectorType>(source);
}


} // namespace Common
} // namespace XT
} // namespace Dune


template <class V, class CharType, class CharTraits>
typename std::enable_if<Dune::XT::Common::is_vector<V>::value && !Dune::XT::Common::VectorAbstraction<V>::has_ostream,
                        std::basic_ostream<CharType, CharTraits>&>::type
operator<<(std::basic_ostream<CharType, CharTraits>& out, const V& vec)
{
  using Vector = Dune::XT::Common::VectorAbstraction<V>;
  if (vec.size() == 0)
    out << "[]";
  else if (vec.size() == 1)
    out << Vector::get_entry(vec, 0);
  else {
    out << "[" << Vector::get_entry(vec, 0);
    for (decltype(vec.size()) ii = 1; ii < vec.size(); ++ii)
      out << " " << Vector::get_entry(vec, ii);
    out << "]";
  }
  return out;
} // ... operator<<(...)


namespace std {


/// clang 3.6 does not consider the overload in the ns for some reason during resultion of a call in gtest
template <class V, class Alloc, class CharType, class CharTraits>
std::basic_ostream<CharType, CharTraits>& operator<<(std::basic_ostream<CharType, CharTraits>& out,
                                                     const std::vector<V, Alloc>& vec)
{
  ::operator<<(out, vec);
  return out;
} // ... operator<<(...)


/// clang 3.6 does not consider the overload in the ns for some reason during resultion of a call in gtest
template <class CharType, class CharTraits, typename _Tp, std::size_t _Nm>
std::basic_ostream<CharType, CharTraits>& operator<<(std::basic_ostream<CharType, CharTraits>& out,
                                                     const std::array<_Tp, _Nm>& vec)
{
  Dune::operator<<(out, vec);
  return out;
} // ... operator<<(...)


} // namespace std

#endif // DUNE_XT_COMMON_VECTOR_HH
