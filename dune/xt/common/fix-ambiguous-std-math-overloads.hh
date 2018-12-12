// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2018)

/// \file Should be included before any other DUNE header.
///
/// Using e.g. the DynamicVector with a field type of long unsigned int failes in dune/common/densevector.hh in
/// one_norm() due to ambiguity of std::abs. We thus need the dirty hacks below and we need them to be included before
/// anyone else includes any DUNE header which pulls dune/common/densevector.hh.

#ifndef DUNE_XT_COMMON_FIX_AMBIGUOUS_STD_MATH_OVERLOADS_HH
#define DUNE_XT_COMMON_FIX_AMBIGUOUS_STD_MATH_OVERLOADS_HH

#include <stdexcept>
#include <cstdint>

namespace Dune {


// forwards, include are below
template <int k>
class bigunsignedint;

class NotImplemented;


} // namespace Dune


namespace std {


long unsigned int abs(const long unsigned int& value);


unsigned char abs(unsigned char value);


template <int k>
Dune::bigunsignedint<k> abs(const Dune::bigunsignedint<k>& value)
{
  return value;
}


template <int k>
inline Dune::bigunsignedint<k> pow(Dune::bigunsignedint<k> /*value*/, std::uintmax_t /*n*/)
{
  // do not pull in any DUNE header for DUNE_THROW
  throw std::runtime_error(
      "[dune/xt/common/fix-ambiguous-std-math-overloads.hh]: pow not implemented for bigunisgnedint!");
  return Dune::bigunsignedint<k>();
}


template <int k>
inline Dune::bigunsignedint<k> sqrt(Dune::bigunsignedint<k> value)
{
  // do not pull in any DUNE header for DUNE_THROW
  throw std::runtime_error(
      "[dune/xt/common/fix-ambiguous-std-math-overloads.hh]: sqrt not implemented for bigunisgnedint!");
  return Dune::bigunsignedint<k>(std::sqrt(value.todouble()));
}


template <int k>
inline Dune::bigunsignedint<k> conj(Dune::bigunsignedint<k> value)
{
  return value;
}


template <int k>
inline bool isnan(Dune::bigunsignedint<k> /*value*/)
{
  return false;
}


template <int k>
inline bool isinf(Dune::bigunsignedint<k> /*value*/)
{
  return false;
}


} // namespace std


#include <dune/common/bigunsignedint.hh>

#endif // DUNE_XT_COMMON_FIX_AMBIGUOUS_STD_MATH_OVERLOADS_HH
