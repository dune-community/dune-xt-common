// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
#define DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH

#include <iostream>

#include <dune/common/densevector.hh>
#include <dune/common/dynvector.hh>
#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/fvector.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/deprecated.hh>

#include <dune/stuff/la/container/interfaces.hh>
#include <dune/stuff/la/container/eigen.hh>
#include "float_cmp.hh"

namespace Dune {
namespace Stuff {
namespace Common {

template <class VectorImp>
inline void DUNE_DEPRECATED_MSG("Use *= 0 instead!") clear(Dune::DenseVector<VectorImp>& vector)
{
  vector *= typename Dune::DenseVector<VectorImp>::value_type(0);
}

template <class T>
inline void DUNE_DEPRECATED_MSG("Use *= 0 instead!") clear(LA::VectorInterface<T>& vector)
{
  vector *= typename LA::VectorInterface<T>::ScalarType(0);
}


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_TOOLS_COMMON_VEECTOR_HH
