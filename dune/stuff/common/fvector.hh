// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FVECTOR_HH
#define DUNE_STUFF_COMMON_FVECTOR_HH

#include <initializer_list>
#include <vector>

#include <dune/common/fvector.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/vector.hh>


namespace Dune {
namespace Stuff {
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

  FieldVector(const size_t UNUSED_UNLESS_DEBUG(sz), const K kk)
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
}; // class FieldVector

template <class K, int SIZE, K value>
class ValueInitFieldVector : public Dune::Stuff::Common::FieldVector<K, SIZE>
{
  static_assert(SIZE >= 0, "Really?");

  typedef Dune::Stuff::Common::FieldVector<K, SIZE> BaseType;
  typedef ValueInitFieldVector<K, SIZE, value> ThisType;

public:
  ValueInitFieldVector()
    : BaseType(value)
  {
  }

}; // class FieldVector


//! Specialization of VectorAbstraction for Dune::Stuff::Common::FieldVector
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


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FVECTOR_HH
