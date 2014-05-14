// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_FMATRIX_HH
#define DUNE_STUFF_COMMON_FMATRIX_HH

#include <dune/common/fmatrix.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/exceptions.hh>

namespace Dune {
namespace Stuff {
namespace Common {


template <class K, int ROWS, int COLS>
class FieldMatrix : public Dune::FieldMatrix<K, ROWS, COLS>
{
  typedef Dune::FieldMatrix<K, ROWS, COLS> BaseType;
  typedef FieldMatrix<K, ROWS, COLS> ThisType;

public:
  FieldMatrix(const size_t rr, const size_t cc, const K kk = K(0))
    : BaseType(kk)
  {
    if (rr != ROWS || cc != COLS)
      DUNE_THROW_COLORFULLY(Exceptions::wrong_input_given,
                            "You are trying to construct a FieldMatrix< ..., " << ROWS << ", " << COLS << " > (of "
                                                                               << "static size) with "
                                                                               << rr
                                                                               << " rows and "
                                                                               << cc
                                                                               << " columns!");
  } // ... FieldMatrix(...)

  FieldMatrix(const BaseType& other)
    : BaseType(other)
  {
  }

  ThisType& operator=(const BaseType& other)
  {
    return BaseType::operator=(other);
  }

  FieldVector<K, ROWS> operator*(const FieldVector<K, COLS>& vec) const
  {
    FieldVector<K, ROWS> ret;
    this->mv(vec, ret);
    return ret;
  } // ... operator*(...)

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
