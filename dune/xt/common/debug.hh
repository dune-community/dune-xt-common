// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014, 2016 - 2017)
//   René Fritze     (2010 - 2018)
//   Tobias Leibner  (2018)

#ifndef DUNE_XT_COMMON_DEBUG_HH
#define DUNE_XT_COMMON_DEBUG_HH

#include <cstring>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include <dune/xt/common/unused.hh>
#include <dune/xt/common/exceptions.hh>


#define SEGFAULT                                                                                                       \
  {                                                                                                                    \
    int* J = 0;                                                                                                        \
    *J = 9;                                                                                                            \
  }


inline char* charcopy(const char* s)
{
  size_t l = strlen(s) + 1;
  char* t = new char[l];
  for (size_t i = 0; i < l; i++) {
    t[i] = s[i];
  }
  return t;
} // ... charcopy(...)


DXT_DEPRECATED_MSG("22.08.18") inline std::string __depr_class()
{
  DUNE_THROW(Dune::NotImplemented, "use the DXT::Typename mechanism instead");
}
#define __CLASS__ __depr_class()

//! try to ensure var is not optimized out
#define DXTC_DEBUG_AUTO(name) volatile auto DXTC_UNUSED(name)

#ifndef NDEBUG
#define DXT_ASSERT(condition)                                                                                          \
  DUNE_THROW_IF(!(condition),                                                                                          \
                Dune::XT::Common::Exceptions::debug_assertion,                                                         \
                __PRETTY_FUNCTION__ << "\nAssertion failed: \n"                                                        \
                                    << #condition)
#else
#define DXT_ASSERT(condition)
#endif

#endif // DUNE_XT_COMMON_DEBUG_HH
