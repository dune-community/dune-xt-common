# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Rene Milk      (2018)
#   Tobias Leibner (2017 - 2018)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# ~~~

include(Hints)

find_library(MATEXP_LIBRARY matrix_exponential HINTS ${lib_hints})
if("${MATEXP_LIBRARY}" MATCHES "MATEXP_LIBRARY-NOTFOUND")
  message(
    "--   library 'matrix_exponential' not found, make sure you have downloaded and build the external libraries!")
else("${MATEXP_LIBRARY}" MATCHES "MATEXP_LIBRARY-NOTFOUND")
  message("--   found matrix_exponential library")
  set(MATEXP_LIBRARIES "${MATEXP_LIBRARY}")
endif("${MATEXP_LIBRARY}" MATCHES "MATEXP_LIBRARY-NOTFOUND")

message("-- checking for matrix_exponential.hpp header")
find_path(MATEXP_INCLUDE_DIRS matrix_exponential/matrix_exponential.hpp HINTS ${include_hints})
if("${MATEXP_INCLUDE_DIRS}" MATCHES "MATEXP_INCLUDE_DIRS-NOTFOUND")
  message("--   matrix_exponential.hpp header not found")
else("${MATEXP_INCLUDE_DIRS}" MATCHES "MATEXP_INCLUDE_DIRS-NOTFOUND")
  message("--   found matrix_exponential.hpp header")
  include_sys_dir(${MATEXP_INCLUDE_DIRS})
endif("${MATEXP_INCLUDE_DIRS}" MATCHES "MATEXP_INCLUDE_DIRS-NOTFOUND")

set(MATEXP_FOUND 0)
if(NOT "${MATEXP_LIBRARY}" MATCHES "MATEXP_LIBRARY-NOTFOUND")
  if(NOT "${MATEXP_INCLUDE_DIRS}" MATCHES "MATEXP_INCLUDE_DIRS-NOTFOUND")
    set(MATEXP_FOUND 1)
  endif(NOT "${MATEXP_INCLUDE_DIRS}" MATCHES "MATEXP_INCLUDE_DIRS-NOTFOUND")
endif(NOT "${MATEXP_LIBRARY}" MATCHES "MATEXP_LIBRARY-NOTFOUND")

set(HAVE_MATEXP ${MATEXP_FOUND})

# register all MATEXP related flags
if(MATEXP_FOUND)
  dune_register_package_flags(LIBRARIES "${MATEXP_LIBRARIES}" INCLUDE_DIRS "${MATEXP_INCLUDE_DIRS}")
endif()
