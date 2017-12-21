# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Tobias Leibner (2017)

find_library(FEKETE_LIBRARY fekete "${CMAKE_SOURCE_DIR}/../local/lib/")
if("${FEKETE_LIBRARY}" MATCHES "FEKETE_LIBRARY-NOTFOUND")
  message("--   library 'fekete' not found, make sure you have downloaded and build the external libraries!")
else("${FEKETE_LIBRARY}" MATCHES "FEKETE_LIBRARY-NOTFOUND")
  message("--   found fekete library")
  set(FEKETE_LIBRARIES "${FEKETE_LIBRARY}")
endif("${FEKETE_LIBRARY}" MATCHES "FEKETE_LIBRARY-NOTFOUND")

message("-- checking for fekete.hpp header")
find_path(FEKETE_INCLUDE_DIRS fekete.hpp HINTS "${CMAKE_SOURCE_DIR}/../local/include/")
if("${FEKETE_INCLUDE_DIRS}" MATCHES "FEKETE_INCLUDE_DIRS-NOTFOUND")
  message("--   fekete.hpp header not found")
else("${FEKETE_INCLUDE_DIRS}" MATCHES "FEKETE_INCLUDE_DIRS-NOTFOUND")
  message("--   found fekete.hpp header")
  include_sys_dir(${FEKETE_INCLUDE_DIRS})
endif("${FEKETE_INCLUDE_DIRS}" MATCHES "FEKETE_INCLUDE_DIRS-NOTFOUND")

set(FEKETE_FOUND 0)
if(NOT "${FEKETE_LIBRARY}" MATCHES "FEKETE_LIBRARY-NOTFOUND")
  if(NOT "${FEKETE_INCLUDE_DIRS}" MATCHES "FEKETE_INCLUDE_DIRS-NOTFOUND")
    set(FEKETE_FOUND 1)
  endif(NOT "${FEKETE_INCLUDE_DIRS}" MATCHES "FEKETE_INCLUDE_DIRS-NOTFOUND")
endif(NOT "${FEKETE_LIBRARY}" MATCHES "FEKETE_LIBRARY-NOTFOUND")

set(HAVE_FEKETE ${FEKETE_FOUND})

# register all FEKETE related flags
if(FEKETE_FOUND)
  dune_register_package_flags(
    LIBRARIES "${FEKETE_LIBRARIES}"
    INCLUDE_DIRS "${FEKETE_INCLUDE_DIRS}")
endif()
