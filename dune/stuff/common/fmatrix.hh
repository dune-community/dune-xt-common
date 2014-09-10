// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FMATRIX_HH
#define DUNE_STUFF_COMMON_FMATRIX_HH

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/fvector.hh>
#include <dune/stuff/common/reenable_warnings.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/fvector.hh>

namespace Dune {
namespace Stuff {
namespace Common {


template <class K, int ROWS, int COLS>
class FieldMatrix : public Dune::FieldMatrix<K, ROWS, COLS>
{
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  FieldMatrix(const K kk = K(0))
    : BaseType(kk)
  {
  } // ... FieldMatrix(...)

  FieldMatrix(const size_t UNUSED_UNLESS_DEBUG(rr), const size_t UNUSED_UNLESS_DEBUG(cc), const K kk = K(0))
    : BaseType(kk)
  {
#ifndef NDEBUG
    if (rr != ROWS || cc != COLS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                                    << "static size) with "
                                                                    << rr
                                                                    << " rows and "
                                                                    << cc
                                                                    << " columns!");
#endif // NDEBUG
  } // ... FieldMatrix(...)

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  Dune::FieldVector<K, ROWS> operator*(const Dune::FieldVector<K, COLS>& vec) const
  {
    Dune::FieldVector<K, ROWS> ret;
    this->mv(vec, ret);
    return ret;
  } // ... operator*(...)

  Dune::FieldVector<K, ROWS> operator*(const FieldMatrix<K, 1, COLS>& mat) const
  {
    Dune::FieldVector<K, ROWS> ret;
    this->mv(mat[0], ret);
    return ret;
  } // ... operator*(...)

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  } // ... operator*(...)

}; // class FieldMatrix


template <class K>
class FieldMatrix<K, 1, 1> : public Dune::FieldMatrix<K, 1, 1>
{
  static const int ROWS = 1;
  static const int COLS = 1;
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  FieldMatrix(const K kk = K(0))
    : BaseType(kk)
  {
  }

  FieldMatrix(const size_t UNUSED_UNLESS_DEBUG(rr), const size_t UNUSED_UNLESS_DEBUG(cc), const K kk = K(0))
    : BaseType(kk)
  {
#ifndef NDEBUG
    if (rr != ROWS || cc != COLS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                                    << "static size) with "
                                                                    << rr
                                                                    << " rows and "
                                                                    << cc
                                                                    << " columns!");
#endif // NDEBUG
  } // ... FieldMatrix(...)

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  FieldMatrix(const Dune::Stuff::Common::FieldVector<K, 1>& other)
    : BaseType(other[0])
  {
  }

  FieldMatrix(const Dune::FieldVector<K, 1>& other)
    : BaseType(other[0])
  {
  }

  using BaseType::operator=;

  ThisType& operator=(const FieldVector<K, 1>& other)
  {
    BaseType::operator=(other[0]);
    return *this;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  } // ... operator*(...)

}; // class FieldMatrix


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FMATRIX_HH
