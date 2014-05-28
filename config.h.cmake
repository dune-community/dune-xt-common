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

#define HAVE_LIKWID ${HAVE_LIKWID}
#define ENABLE_PERFMON ${ENABLE_PERFMON}
#if ENABLE_PERFMON && HAVE_LIKWID
# define LIKWID_PERFMON 1
#endif

#define DS_OVERRIDE ${DS_OVERRIDE}
#define DS_FINAL  ${DS_FINAL}
#define HAVE_DUNE_FEM_PARAMETER_REPLACE 0

#ifndef HAVE_DUNE_FEM
# define HAVE_FUNE_FEM 0
# endif

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

#ifndef HAVE_CONSTEXPR
# define HAVE_CONSTEXPR 0
#endif
/* end dune-stuff */
// NEVER delete/alter above comment, dune's cmake crap relies on it
