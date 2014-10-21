/* begin dune-stuff */
// NEVER delete/alter above comment, dune's cmake crap relies on it

/* We need this for the python bindings (there is no unsigned in python). */
#define DUNE_STUFF_SSIZE_T long int

/* Define to the version of dune-stuff */
#define DUNE_STUFF_VERSION "${DUNE_STUFF_VERSION}"

/* Define to the major version of dune-stuff */
#define DUNE_STUFF_VERSION_MAJOR ${DUNE_STUFF_VERSION_MAJOR}

/* Define to the minor version of dune-stuff */
#define DUNE_STUFF_VERSION_MINOR ${DUNE_STUFF_VERSION_MINOR}

/* Define to the revision of dune-stuff */
#define DUNE_STUFF_VERSION_REVISION ${DUNE_STUFF_VERSION_REVISION}

/* Define to 1 if eigen was found, else 0 */
#define HAVE_EIGEN ${HAVE_EIGEN}

/* Define to 1 if threading building blocks were found, else 0 */
#define HAVE_TBB ${HAVE_TBB}

#define HAVE_LIKWID ${HAVE_LIKWID}
#define ENABLE_PERFMON ${ENABLE_PERFMON}
#if ENABLE_PERFMON && HAVE_LIKWID
# define LIKWID_PERFMON 1
#endif

#define DS_OVERRIDE ${DS_OVERRIDE}
#define DS_FINAL  ${DS_FINAL}
#define HAVE_DUNE_FEM_PARAMETER_REPLACE 0

#ifndef HAVE_ALUGRID
#  define DUNE_FEM_DONT_WARN_IN_COMM_MANAGER 1
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

#ifndef BOOST_CLANG
# define BOOST_CLANG 0
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

/*** End: Silence implicitly False evaluation of undefined macro warnings ****/

/* end dune-stuff */
// NEVER delete/alter above comment, dune's cmake crap relies on it
