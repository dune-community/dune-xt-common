// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Barbara Verfürth (2015)
//   Felix Schindler  (2012 - 2016)
//   Rene Milk        (2012 - 2015)
//   Sven Kaulmann    (2013)
//   Tobias Leibner   (2014 - 2015)

#ifndef DUNE_XT_COMMON_VECTOR_HH
#define DUNE_XT_COMMON_VECTOR_HH

#include <vector>
#include <ostream>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>
#include <dune/common/ftraits.hh>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/type_traits.hh>
#include <dune/xt/common/float_cmp.hh>

namespace Dune {
namespace XT {
namespace Common {

template <class VecType>
struct VectorAbstraction;

//! logically and structurally this belongs in type_utils.hh, but the dependent implementation prohibits that
template <class VectorType>
struct is_vector
{
  static const bool value = VectorAbstraction<VectorType>::is_vector;
};

/**
 * \brief Traits to statically extract information of a (mathematical) vector.
 *
 *        If you want your vector class to benefit from the operators defined in this header you have to manually
 *        specify a specialization of this class in your code with is_vector defined to true and the appropriate
 *        static methods and members (see the specializations below).
 */
template <class VecType>
struct VectorAbstraction
{
  typedef VecType VectorType;
  typedef VecType ScalarType;
  typedef VecType RealType;
  typedef typename Dune::FieldTraits<VecType>::field_type S;
  typedef typename Dune::FieldTraits<VecType>::real_type R;

  static constexpr bool is_vector = false;

  static constexpr bool has_static_size = false;

  static constexpr size_t static_size = std::numeric_limits<size_t>::max();

  static inline /*VectorType*/ void create(const size_t /*sz*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }

  static inline /*VectorType*/ void create(const size_t /*sz*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<VecType>::value, "Do not call me if is_vector is false!");
  }
};

template <class T>
struct VectorAbstraction<std::vector<T>>
{
  typedef std::vector<T> VectorType;
  typedef typename Dune::FieldTraits<T>::field_type ScalarType;
  typedef typename Dune::FieldTraits<T>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static constexpr bool is_vector = true;

  static constexpr bool has_static_size = false;

  static constexpr size_t static_size = std::numeric_limits<size_t>::max();

  static inline VectorType create(const size_t sz)
  {
    return VectorType(sz);
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    return VectorType(sz, val);
  }
};

template <class K>
struct VectorAbstraction<Dune::DynamicVector<K>>
{
  typedef Dune::DynamicVector<K> VectorType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static constexpr bool is_vector = true;

  static constexpr bool has_static_size = false;

  static constexpr size_t static_size = std::numeric_limits<size_t>::max();

  static inline VectorType create(const size_t sz)
  {
    return VectorType(sz);
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    return VectorType(sz, val);
  }
};

template <class K, int SIZE>
struct VectorAbstraction<Dune::FieldVector<K, SIZE>>
{
  typedef Dune::FieldVector<K, SIZE> VectorType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static constexpr bool is_vector = true;

  static constexpr bool has_static_size = true;

  static constexpr size_t static_size = SIZE;

  static inline VectorType create(const size_t sz)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "sz = " << sz << "\nSIZE = " << int(SIZE));
    return VectorType();
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    if (sz != SIZE)
      DUNE_THROW(Exceptions::shapes_do_not_match, "sz = " << sz << "\nSIZE = " << int(SIZE));
    return VectorType(val);
  }
};

template <class T>
struct VectorAbstraction<std::complex<T>>
{
  typedef std::complex<T> VectorType;
  typedef std::complex<T> ScalarType;
  typedef T RealType;
  typedef ScalarType S;
  typedef RealType R;

  static constexpr bool is_vector = false;

  static constexpr bool has_static_size = true;

  static constexpr size_t static_size = 2u;

  static inline VectorType create(const size_t /*sz*/)
  {
    return VectorType(0, 0);
  }

  static inline VectorType create(const size_t /*sz*/, const ScalarType& val)
  {
    return VectorType(val);
  }
  static inline VectorType create(const size_t /*sz*/, const RealType& val)
  {
    return VectorType(val, val);
  }
};

template <class VectorType>
typename std::enable_if<is_vector<VectorType>::value, VectorType>::type
create(const size_t sz,
       const typename VectorAbstraction<VectorType>::S& val = typename VectorAbstraction<VectorType>::S(0))
{
  return VectorAbstraction<VectorType>::create(sz, val);
}

template <class T, class SR>
typename std::enable_if<is_complex<T>::value, T>::type create(const size_t /*sz*/,
                                                              const SR& val = typename VectorAbstraction<T>::R(0))
{
  return VectorAbstraction<T>::create(0, val);
}

template <class VectorType>
typename std::enable_if<std::is_arithmetic<VectorType>::value, VectorType>::type
create(const size_t /*sz*/,
       const typename VectorAbstraction<VectorType>::S& val = typename VectorAbstraction<VectorType>::S(0))
{
  return val;
}

} // namespace Common
} // namespace XT
} // namespace Dune

template <class S, class V>
typename std::enable_if<std::is_arithmetic<S>::value && Dune::XT::Common::is_vector<V>::value, V>::type
operator*(const S& scalar, const V& vec)
{
  V result(vec);
  for (size_t ii = 0; ii < vec.size(); ++ii)
    result[ii] *= scalar;
  return result;
} // ... operator*(...)

template <class L, class R>
typename std::enable_if<Dune::XT::Common::is_vector<L>::value && Dune::XT::Common::is_vector<R>::value
                            && std::is_same<typename Dune::XT::Common::VectorAbstraction<L>::S,
                                            typename Dune::XT::Common::VectorAbstraction<R>::S>::value,
                        L>::type
operator+(const L& left, const R& right)
{
  const auto sz = left.size();
  if (right.size() != sz)
    DUNE_THROW(Dune::XT::Common::Exceptions::shapes_do_not_match,
               "left.size() = " << sz << "\nright.size() = " << right.size());
  L result(left);
  for (size_t ii = 0; ii < sz; ++ii)
    result[ii] += right[ii];
  return result;
} // ... operator+(...)

template <class V, class CharType, class CharTraits>
typename std::enable_if<Dune::XT::Common::is_vector<V>::value, std::basic_ostream<CharType, CharTraits>&>::type
operator<<(std::basic_ostream<CharType, CharTraits>& out, const V& vec)
{
  if (vec.size() == 0)
    out << "[]";
  else if (vec.size() == 1)
    out << vec[0];
  else {
    out << "[" << vec[0];
    for (decltype(vec.size()) ii = 1; ii < vec.size(); ++ii)
      out << " " << vec[ii];
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
}

#endif // DUNE_XT_COMMON_VECTOR_HH
