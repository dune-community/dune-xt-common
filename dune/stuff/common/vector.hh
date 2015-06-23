// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TOOLS_COMMON_VECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VECTOR_HH

#include <vector>
#include <ostream>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>
#include <dune/common/ftraits.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/type_utils.hh>


namespace Dune {
namespace Stuff {
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
 *        specify a specialization of this class in your code with is_vector defined to true and an appropriate
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
  typedef typename Dune::FieldTraits<T>::field_type ScalarType;
  typedef typename Dune::FieldTraits<T>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

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
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

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
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

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
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

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
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
    operator<(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::lt(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
    operator>(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::gt(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
    operator<=(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::le(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
    operator>=(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::ge(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
    operator==(const L& lhs, const R& rhs)
{
  return Dune::Stuff::Common::FloatCmp::eq(lhs, rhs);
}

template <class L, class R>
inline typename std::enable_if<Dune::Stuff::Common::is_vector<L>::value && Dune::Stuff::Common::is_vector<R>::value,
                               bool>::value
    DUNE_DEPRECATED_MSG("vector operator overloads to be removed. If you want FloatCmp bevahior cal the appropiate "
                        "DSC::FloatCmp::XX function instead")
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


template <class V>
typename std::enable_if<Dune::Stuff::Common::is_vector<V>::value, std::ostream&>::type operator<<(std::ostream& out,
                                                                                                  const V& vec)
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


#endif // DUNE_STUFF_TOOLS_COMMON_VECTOR_HH
