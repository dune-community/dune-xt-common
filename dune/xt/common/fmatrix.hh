// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016 - 2017)
//   Rene Milk       (2015 - 2016)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_FMATRIX_HH
#define DUNE_XT_COMMON_FMATRIX_HH

#include <initializer_list>

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
  FieldMatrix(const K& kk = 0)
    : BaseType(kk)
  {
  }

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K& kk = 0)
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

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType(0.)
  {
#ifndef NDEBUG
    if (list_of_rows.size() != ROWS)
      DUNE_THROW(
          Exceptions::wrong_input_given,
          "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                             << "static size) from a list modeling a matrix with "
                                                             << list_of_rows.size()
                                                             << " rows!");
#endif // NDEBUG
    size_t rr = 0;
    for (auto row : list_of_rows) {
#ifndef NDEBUG
      if (row.size() != COLS)
        DUNE_THROW(
            Exceptions::wrong_input_given,
            "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                               << "static size) from a list with a row of length "
                                                               << row.size()
                                                               << "!");
#endif // NDEBUG
      size_t cc = 0;
      for (auto entry : row)
        (*this)[rr][cc++] = entry;
      ++rr;
    }
  } // FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)

  FieldMatrix(const ThisType& other) = default;

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  Dune::XT::Common::FieldMatrix<K, COLS, ROWS> transpose() const
  {
    Dune::XT::Common::FieldMatrix<K, COLS, ROWS> ret;
    for (size_t rr = 0; rr < ROWS; ++rr)
      for (size_t cc = 0; cc < COLS; ++cc)
        ret[cc][rr] = (*this)[rr][cc];
    return ret;
  }

  ThisType operator+(const ThisType& other) const
  {
    ThisType ret = *this;
    ret += other;
    return ret;
  }

  ThisType operator-(const ThisType& other) const
  {
    ThisType ret = *this;
    ret -= other;
    return ret;
  }

  template <int O_COLS>
  Dune::XT::Common::FieldMatrix<K, ROWS, O_COLS> operator*(const Dune::FieldMatrix<K, COLS, O_COLS>& mat) const
  {
    return this->rightmultiplyany(mat);
  }

  Dune::XT::Common::FieldVector<K, ROWS> operator*(const Dune::FieldVector<K, COLS>& vec) const
  {
    Dune::FieldVector<K, ROWS> ret;
    this->mv(vec, ret);
    return ret;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  }
}; // class FieldMatrix<...>

template <class K>
class FieldMatrix<K, 1, 1> : public Dune::FieldMatrix<K, 1, 1>
{
  static const int ROWS = 1;
  static const int COLS = 1;
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  FieldMatrix(const K& kk = 0)
    : BaseType(kk)
  {
  }

  FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)
    : BaseType(0.)
  {
#ifndef NDEBUG
    if (list_of_rows.size() != ROWS)
      DUNE_THROW(Exceptions::wrong_input_given,
                 "You are trying to construct a FieldMatrix< ..., 1, 1 > (of "
                     << "static size) from a list modeling a matrix with "
                     << list_of_rows.size()
                     << " rows!");
#endif // NDEBUG
    for (auto row : list_of_rows) {
#ifndef NDEBUG
      if (row.size() != COLS)
        DUNE_THROW(Exceptions::wrong_input_given,
                   "You are trying to construct a FieldMatrix< ..., 1, 1 > (of "
                       << "static size) from a list with a row of length "
                       << row.size()
                       << "!");
#endif // NDEBUG
      for (auto entry : row)
        (*this)[0][0] = entry;
    }
  } // FieldMatrix(std::initializer_list<std::initializer_list<K>> list_of_rows)

  FieldMatrix(const size_t DXTC_DEBUG_ONLY(rr), const size_t DXTC_DEBUG_ONLY(cc), const K& kk = 0)
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

  ThisType operator-(const ThisType& other) const
  {
    ThisType ret = *this;
    ret -= other;
    return ret;
  }

  ThisType operator*(const K& scal) const
  {
    ThisType ret(*this);
    ret *= scal;
    return ret;
  }
}; // class FieldMatrix


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_FMATRIX_HH
