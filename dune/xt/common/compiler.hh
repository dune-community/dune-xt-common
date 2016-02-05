// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016)
//   Rene Milk       (2015)

#ifndef DUNE_XT_COMMON_COMPILER_HH
#define DUNE_XT_COMMON_COMPILER_HH

#if defined(__GNUC__)
#if defined(__GNUC_PATCHLEVEL__)
#define DXTC_GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define DXTC_GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#endif
#endif

#endif // DUNE_XT_COMMON_COMPILER_HH
