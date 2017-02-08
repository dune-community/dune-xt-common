// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016)
//   Rene Milk       (2015)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_FMATRIX_HH
#define DUNE_XT_COMMON_FMATRIX_HH

#include <dune/common/fmatrix.hh>
#include <dune/common/fvector.hh>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/debug.hh>
#include <dune/xt/common/fvector.hh>

namespace Dune {
namespace XT {
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

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K kk = K(0))
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

  ThisType operator+(const ThisType& other) const
  {
    ThisType ret = *this;
    ret += other;
    return ret;
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

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K kk = K(0))
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

  FieldMatrix(const Dune::XT::Common::FieldVector<K, 1>& other)
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

  ThisType operator+(const ThisType& other) const
  {
    ThisType ret = *this;
    ret += other;
    return ret;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  } // ... operator*(...)

}; // class FieldMatrix

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_FMATRIX_HH
