// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014, 2016 - 2017)
//   Rene Milk       (2010 - 2016)

#ifndef DUNE_XT_COMMON_DEBUG_HH
#define DUNE_XT_COMMON_DEBUG_HH

#include <cstring>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include "unused.hh"


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


#define __CLASS__ strtok(charcopy(__PRETTY_FUNCTION__), "<(")


#ifndef ASSERT_LT
#define ASSERT_LT(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt < actual),                                                                                                 \
      (boost::format("assertion %1% < %2% failed: %3% >= %4%") % #expt % #actual % expt % actual).str().c_str())
#endif


#ifndef ASSERT_EQ
#define ASSERT_EQ(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt == actual),                                                                                                \
      (boost::format("assertion %1% == %2% failed: %3% != %4%") % #expt % #actual % expt % actual).str().c_str())
#endif


#endif // DUNE_XT_COMMON_DEBUG_HH
