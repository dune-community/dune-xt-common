# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

find_path(LIKWID_INCLUDE_DIR NAMES likwid.h PATHS ${LIKWID_ROOT} PATH_SUFFIXES include)
find_library(LIKWID_LIBRARY  NAMES likwid   PATHS ${LIKWID_ROOT} PATH_SUFFIXES lib)

mark_as_advanced(LIKWID_LIBRARY LIKWID_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set LIKWID_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIKWID LIKWID_LIBRARY LIKWID_INCLUDE_DIR)

set(HAVE_LIKWID 0)
if(LIKWID_FOUND)
    set(LIKWID_INCLUDE_DIRS ${LIKWID_INCLUDE_DIR})
    set(LIKWID_LIBRARIES ${LIKWID_LIBRARY})
    set(HAVE_LIKWID 1)
endif()

