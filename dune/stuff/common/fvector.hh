// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FVECTOR_HH
#define DUNE_STUFF_COMMON_FVECTOR_HH

#include <vector>
#include <initializer_list>

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/fvector.hh>
#include <dune/stuff/common/reenable_warnings.hh>

#include <dune/stuff/common/exceptions.hh>

namespace Dune {
namespace Stuff {
namespace Common {


template <class K, int SIZE>
class FieldVector : public Dune::FieldVector<K, SIZE>
{
  typedef Dune::FieldVector<K, SIZE> BaseType;
  typedef FieldVector<K, SIZE> ThisType;

public:
  FieldVector(const K kk = K(0))
    : BaseType(kk)
  {
  }

  FieldVector(const BaseType& other)
    : BaseType(other)
  {
  }

  FieldVector(const std::vector<K>& vec)
    : BaseType(K(0))
  {
#ifndef NDEBUG
    if (vec.size() != SIZE)
      DUNE_THROW_COLORFULLY(Exceptions::wrong_input_given,
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
      DUNE_THROW_COLORFULLY(Exceptions::wrong_input_given,
                            "You are trying to construct a FieldVector< ..., " << SIZE << " > (of "
                                                                               << "static size) from a list of size "
                                                                               << list.size()
                                                                               << "!");
#endif // NDEBUG
    size_t ii = 0;
    for (auto element : list) {
      this->operator[](ii) = element;
      ++ii;
    }
  } // FieldVector(...)

  ThisType& operator=(const BaseType& other)
  {
    return BaseType::operator=(other);
  }
}; // class FieldVector


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FVECTOR_HH
