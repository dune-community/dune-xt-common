// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2013 - 2016)
//   Rene Milk       (2013 - 2015)
//   Tobias Leibner  (2014)

/* begin dune-xt-common */
// NEVER delete/alter above comment, dune's cmake relies on it

/* Define to the version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION ${DUNE_XT_COMMON_VERSION}

/* Define to the major version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_MAJOR ${DUNE_XT_COMMON_VERSION_MAJOR}

/* Define to the minor version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_MINOR ${DUNE_XT_COMMON_VERSION_MINOR}

/* Define to the revision of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_REVISION ${DUNE_XT_COMMON_VERSION_REVISION}

#define HAVE_EIGEN ENABLE_EIGEN

/* Define to 1 if threading building blocks were found, else 0 */
#ifndef HAVE_TBB
#define HAVE_TBB ${HAVE_TBB}
#endif

#ifndef DXT_DISABLE_LARGE_TESTS
#define DXT_DISABLE_LARGE_TESTS 0
#endif

#cmakedefine HAS_WORKING_UNUSED_ATTRIBUTE  1

#define HAVE_LIKWID ${HAVE_LIKWID}
#define ENABLE_PERFMON ${ENABLE_PERFMON}
#if ENABLE_PERFMON && HAVE_LIKWID
# define LIKWID_PERFMON 1
#endif

#cmakedefine HAVE_MAP_EMPLACE 1

#define DS_MAX_MIC_THREADS ${DS_MAX_MIC_THREADS}

#define HAVE_DUNE_FEM_PARAMETER_REPLACE 0

/* needed in dune/xt/common/profiler.hh */
#ifndef DUNE_XT_COMMON_DO_TIMING
# define DUNE_XT_COMMON_DO_TIMING 0
#endif

/*** Silence implicitly False evaluation of undefined macro warnings ****/
#ifndef HAVE_DUNE_FEM
# define HAVE_DUNE_FEM 0
#endif

#ifndef HAVE_DUNE_GRID_MULTISCALE
# define HAVE_DUNE_GRID_MULTISCALE 0
#endif

#ifndef HAVE_ALBERTA
# define HAVE_ALBERTA 0
#endif

#ifndef HAVE_DUNE_SPGRID
# define HAVE_DUNE_SPGRID 0
#endif

#ifndef HAVE_EMPLACE
# define HAVE_EMPLACE 0
#endif

#ifndef ENABLE_SUPERLU
# define ENABLE_SUPERLU 0
#endif

#ifndef HAVE_SUPERLU
# define HAVE_SUPERLU 0
#endif

#ifndef ENABLE_UMFPACK
# define ENABLE_UMFPACK 0
#endif

#ifndef HAVE_UMFPACK
# define HAVE_UMFPACK 0
#endif

#ifndef ENABLE_BOOST
# define ENABLE_BOOST 0
#endif

#ifndef ENABLE_PETSC
# define ENABLE_PETSC 0
#endif

#ifndef ENABLE_MPI
# define ENABLE_MPI 0
#endif

#ifndef HAVE_MPI
# define HAVE_MPI 0
#endif

#ifndef HAVE_CONSTEXPR
# define HAVE_CONSTEXPR 0
#endif

#ifndef HAVE_PETSC
# define HAVE_PETSC 0
#endif

#ifndef DUNE_FEM_COMPATIBILITY
# define DUNE_FEM_COMPATIBILITY 0
#endif

#ifndef HAVE_MAP_EMPLACE
#define HAVE_MAP_EMPLACE 0
#endif

/*** End: Silence implicitly False evaluation of undefined macro warnings ****/

/* end dune-xt-common */
// NEVER delete/alter above comment, dune's cmake relies on it
