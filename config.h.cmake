// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2013 - 2015)
//   Rene Milk       (2013 - 2015)
//   Tobias Leibner  (2014)

/* begin dune-xt-common */
// NEVER delete/alter above comment, dune's cmake crap relies on it

/* We need this for the python bindings (there is no unsigned in python). */
#define DUNE_XT_COMMON_SSIZE_T long int

/* Define to the version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION ${DUNE_XT_COMMON_VERSION}

/* Define to the major version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_MAJOR ${DUNE_XT_COMMON_VERSION_MAJOR}

/* Define to the minor version of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_MINOR ${DUNE_XT_COMMON_VERSION_MINOR}

/* Define to the revision of dune-xt-common */
#define DUNE_XT_COMMON_VERSION_REVISION ${DUNE_XT_COMMON_VERSION_REVISION}

/* Define to 1 if eigen was found, else 0 */
#ifndef HAVE_EIGEN
#define HAVE_EIGEN ${HAVE_EIGEN}
#endif

/* Define to 1 if threading building blocks were found, else 0 */
#ifndef HAVE_TBB
#define HAVE_TBB ${HAVE_TBB}
#endif

#cmakedefine HAS_WORKING_UNUSED_ATTRIBUTE  1

#define HAVE_LIKWID ${HAVE_LIKWID}
#define ENABLE_PERFMON ${ENABLE_PERFMON}
#if ENABLE_PERFMON && HAVE_LIKWID
# define LIKWID_PERFMON 1
#endif

#cmakedefine HAVE_MAP_EMPLACE 1

#define DS_MAX_MIC_THREADS ${DS_MAX_MIC_THREADS}

#define DS_OVERRIDE ; static_assert(false, "Use override instead (21.10.2014)!");
#define DS_FINAL    ; static_assert(false, "Use final instead (21.10.2014)!");
#define HAVE_DUNE_FEM_PARAMETER_REPLACE 0

#ifndef HAVE_ALUGRID
#  define DUNE_FEM_DONT_WARN_IN_COMM_MANAGER 1
#endif

/* needed in dune/xt/common/profiler.hh */
#ifndef DUNE_XT_COMMON_DO_PROFILE
# define DUNE_XT_COMMON_DO_PROFILE 0
#endif

/*** Silence implicitly False evaluation of undefined macro warnings ****/
#ifndef HAVE_DUNE_FEM
# define HAVE_FUNE_FEM 0
#endif

#ifndef HAVE_DUNE_PYMOR
# define HAVE_DUNE_PYMOR 0
#endif

#ifndef HAVE_DUNE_GRID_MULTISCALE
# define HAVE_DUNE_GRID_MULTISCALE 0
#endif

#ifndef HAVE_DUNE_GDT
# define HAVE_DUNE_GDT 0
#endif

#ifndef HAVE_DUNE_HDD
# define HAVE_DUNE_HDD 0
#endif

#ifndef HAVE_EMPLACE
# define HAVE_EMPLACE 0
#endif

#ifndef ENABLE_SUPERLU
# define ENABLE_SUPERLU 0
#endif

#ifndef ENABLE_UMFPACK
# define ENABLE_UMFPACK 0
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

#ifndef HAVE_FASP
# define HAVE_FASP 0
#endif

#ifndef HAVE_CONSTEXPR
# define HAVE_CONSTEXPR 0
#endif

#ifndef HAVE_DUNE_METAGRID
# define HAVE_DUNE_METAGRID 0
#endif 

#ifndef HAVE_DUNE_SPGRID
# define HAVE_DUNE_SPGRID 0
#endif 

#ifndef HAVE_PETSC
# define HAVE_PETSC 0
#endif

#ifndef HAVE_ALUGRID
# define HAVE_ALUGRID 0
#endif

#ifndef DUNE_FEM_COMPATIBILITY
# define DUNE_FEM_COMPATIBILITY 0
#endif

#ifndef HAVE_ALBERTA
# define HAVE_ALBERTA 0
#endif

#ifndef HAVE_UG
# define HAVE_UG 0
#endif

#ifndef HAVE_GRAPE
# define HAVE_GRAPE 0
#endif

#ifndef ENABLE_ALUGRID
# define ENABLE_ALUGRID 0
#endif

#ifndef DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS
# define DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS 0
#endif

#ifndef HAVE_MAP_EMPLACE
#define HAVE_MAP_EMPLACE 0
#endif

/*** End: Silence implicitly False evaluation of undefined macro warnings ****/

/* end dune-xt-common */
// NEVER delete/alter above comment, dune's cmake crap relies on it
