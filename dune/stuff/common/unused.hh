// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_UNUSED_HH
#define DUNE_STUFF_COMMON_UNUSED_HH

#ifdef HAS_WORKING_UNUSED_ATTRIBUTE
#define DSC_UNUSED(identifier) identifier __attribute__((unused))
#else
#define DSC_UNUSED(identifier) /*identifier*/
#endif

#ifndef NDEBUG
#define DSC_DEBUG_ONLY(param) param
#else
#define DSC_DEBUG_ONLY(param) DSC_UNUSED(param)
#endif

/*** LEGACY MACROS ****/
#define UNUSED(identifier) /* identifier */

/*  some arguments are only used in debug mode, but unused in release one */
#ifndef NDEBUG
#define UNUSED_UNLESS_DEBUG(param) param
#else
#define UNUSED_UNLESS_DEBUG(param) UNUSED(param)
#endif // ifndef NDEBUG
/*** LEGACY MACROS ****/

#endif // DUNE_STUFF_COMMON_UNUSED_HH
