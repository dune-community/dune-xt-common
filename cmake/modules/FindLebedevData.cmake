# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Tobias Leibner (2017)

find_library(LEBEDEV_DATA_LIBRARY lebedevdata "${CMAKE_SOURCE_DIR}/../local/lib/")
if("${LEBEDEV_DATA_LIBRARY}" MATCHES "LEBEDEV_DATA_LIBRARY-NOTFOUND")
  message("--   library 'lebedevdata' not found, make sure you have downloaded and build the external libraries!")
else("${LEBEDEV_DATA_LIBRARY}" MATCHES "LEBEDEV_DATA_LIBRARY-NOTFOUND")
  message("--   found lebedevdata library")
  set(LEBEDEV_DATA_LIBRARIES "${LEBEDEV_DATA_LIBRARY}")
endif("${LEBEDEV_DATA_LIBRARY}" MATCHES "LEBEDEV_DATA_LIBRARY-NOTFOUND")

message("-- checking for lebedev_data.hh header")
find_path(LEBEDEV_DATA_INCLUDE_DIRS lebedev_data.hh HINTS "${CMAKE_SOURCE_DIR}/../local/include/")
if("${LEBEDEV_DATA_INCLUDE_DIRS}" MATCHES "LEBEDEV_DATA_INCLUDE_DIRS-NOTFOUND")
  message("--   lebedev_data.hh header not found")
else("${LEBEDEV_DATA_INCLUDE_DIRS}" MATCHES "LEBEDEV_DATA_INCLUDE_DIRS-NOTFOUND")
  message("--   found lebedev_data.hh header")
  include_sys_dir(${LEBEDEV_DATA_INCLUDE_DIRS})
endif("${LEBEDEV_DATA_INCLUDE_DIRS}" MATCHES "LEBEDEV_DATA_INCLUDE_DIRS-NOTFOUND")

set(LEBEDEV_DATA_FOUND 0)
if(NOT "${LEBEDEV_DATA_LIBRARY}" MATCHES "LEBEDEV_DATA_LIBRARY-NOTFOUND")
  if(NOT "${LEBEDEV_DATA_INCLUDE_DIRS}" MATCHES "LEBEDEV_DATA_INCLUDE_DIRS-NOTFOUND")
    set(LEBEDEV_DATA_FOUND 1)
  endif(NOT "${LEBEDEV_DATA_INCLUDE_DIRS}" MATCHES "LEBEDEV_DATA_INCLUDE_DIRS-NOTFOUND")
endif(NOT "${LEBEDEV_DATA_LIBRARY}" MATCHES "LEBEDEV_DATA_LIBRARY-NOTFOUND")

set(HAVE_LEBEDEV_DATA ${LEBEDEV_DATA_FOUND})

# register all LEBEDEV_DATA related flags
if(LEBEDEV_DATA_FOUND)
  dune_register_package_flags(
    LIBRARIES "${LEBEDEV_DATA_LIBRARIES}"
    INCLUDE_DIRS "${LEBEDEV_DATA_INCLUDE_DIRS}")
endif()
