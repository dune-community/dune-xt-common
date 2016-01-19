// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2016)
//   Rene Milk       (2015)

#ifndef DXTC_UNUSED_HH
#define DXTC_UNUSED_HH

#ifdef HAS_WORKING_UNUSED_ATTRIBUTE
#define DXTC_UNUSED(identifier) identifier __attribute__((unused))
#else
#define DXTC_UNUSED(identifier) /*identifier*/
#endif

#ifndef NDEBUG
#define DXTC_DEBUG_ONLY(param) param
#else
#define DXTC_DEBUG_ONLY(param) DXTC_UNUSED(param)
#endif

#endif // DXTC_UNUSED_HH
