// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2015)

#ifndef DUNE_XT_COMMON_COMPILER_HH
#define DUNE_XT_COMMON_COMPILER_HH

#if defined(__GNUC__)
#if defined(__GNUC_PATCHLEVEL__)
#define DS_GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define DS_GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#endif
#endif

#endif // DUNE_XT_COMMON_COMPILER_HH
