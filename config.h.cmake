/* begin dune-stuff */

/* Define to the version of dune-stuff */
#define DUNE_STUFF_VERSION "${DUNE_STUFF_VERSION}"

/* Define to the major version of dune-stuff */
#define DUNE_STUFF_VERSION_MAJOR ${DUNE_STUFF_VERSION_MAJOR}

/* Define to the minor version of dune-stuff */
#define DUNE_STUFF_VERSION_MINOR ${DUNE_STUFF_VERSION_MINOR}

/* Define to the revision of dune-stuff */
#define DUNE_STUFF_VERSION_REVISION ${DUNE_STUFF_VERSION_REVISION}

#define HAVE_LIKWID ${HAVE_LIKWID}
#define ENABLE_PERFMON ${ENABLE_PERFMON}
#if ENABLE_PERFMON && HAVE_LIKWID
# define LIKWID_PERFMON 1
#endif

#define DS_OVERRIDE ${DS_OVERRIDE}
#define DS_FINAL  ${DS_FINAL}
/* end dune-stuff */