# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2013 - 2014, 2016 - 2017)
#   Rene Milk       (2013, 2015)

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

