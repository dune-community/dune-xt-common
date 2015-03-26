// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TOOLS_COMMON_VECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VECTOR_HH

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/type_utils.hh>


namespace Dune {
namespace Stuff {
namespace Common {


/**
 * \brief Traits to statically extract the scalar type of a (mathematical) vector-
 *
 *        If you want your vector class to benefit from the operators defined in this header you have to manually
 *        specify a specialization of this class in your code with is_vector defined to true and an appropriate
 *        static create() method (see the specializations below).
 */
template <class VecType>
struct VectorAbstraction
{
  typedef VecType VectorType;
  typedef VecType ScalarType;
  typedef VecType S;

  static const bool is_vector = false;

  static const bool has_static_size = false;

  static const size_t static_size = std::numeric_limits<size_t>::max();

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
  typedef T ScalarType;
  typedef ScalarType S;

  static const bool is_vector = true;

  static const bool has_static_size = false;

  static const size_t static_size = std::numeric_limits<size_t>::max();

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
  typedef K ScalarType;
  typedef ScalarType S;

  static const bool is_vector = true;

  static const bool has_static_size = false;

  static const size_t static_size = std::numeric_limits<size_t>::max();

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
  typedef K ScalarType;
  typedef ScalarType S;

  static const bool is_vector = true;

  static const bool has_static_size = true;

  static const size_t static_size = SIZE;

  static inline VectorType create(const size_t sz)
  {
    if (sz != SIZE)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "sz = " << sz << "\nSIZE = " << int(SIZE));
    return VectorType();
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    if (sz != SIZE)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "sz = " << sz << "\nSIZE = " << int(SIZE));
    return VectorType(val);
  }
};

template <class K, int SIZE>
struct VectorAbstraction<Dune::Stuff::Common::FieldVector<K, SIZE>>
{
  typedef Dune::Stuff::Common::FieldVector<K, SIZE> VectorType;
  typedef K ScalarType;
  typedef ScalarType S;

  static const bool is_vector = true;

  static const bool has_static_size = true;

  static const size_t static_size = SIZE;

  static inline VectorType create(const size_t sz)
  {
    return VectorType(sz);
  }

  static inline VectorType create(const size_t sz, const ScalarType& val)
  {
    return VectorType(sz, val);
  }
};


template <class VectorType>
struct is_vector
{
  static const bool value = VectorAbstraction<VectorType>::is_vector;
};


template <class VectorType>
typename std::enable_if<is_vector<VectorType>::value, VectorType>::type
create(const size_t sz,
       const typename VectorAbstraction<VectorType>::S& val = typename VectorAbstraction<VectorType>::S(0))
{
  return VectorAbstraction<VectorType>::create(sz, val);
}


} // namespace Common
} // namespace Stuff
} // namespace Dune


template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator<(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::lt(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator>(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::gt(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator<=(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::le(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator>=(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::ge(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator==(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::eq(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
operator!=(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::ne(lhs, rhs);
}


template <class S, class V>
typename std::enable_if<std::is_arithmetic<S>::value && Dune::Stuff::Common::is_vector<V>::value, V>::type
operator*(const S& scalar, const V& vec)
{
  V result(vec);
  for (size_t ii = 0; ii < vec.size(); ++ii)
    result[ii] *= scalar;
  return result;
} // ... operator*(...)


template <class L, class R>
typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value
                            && std::is_same<typename Dune::Stuff::Common::VectorAbstraction<L>::S,
                                            typename Dune::Stuff::Common::VectorAbstraction<R>::S>::value,
                        L>::type
operator+(const L& left, const R& right)
{
  const auto sz = left.size();
  if (right.size() != sz)
    DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match,
               "left.size() = " << sz << "\nright.size() = " << right.size());
  L result(left);
  for (size_t ii = 0; ii < sz; ++ii)
    result[ii] += right[ii];
  return result;
} // ... operator+(...)


#endif // DUNE_STUFF_TOOLS_COMMON_VECTOR_HH
