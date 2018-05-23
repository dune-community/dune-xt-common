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

find_library(LEBEDEVDATA_LIBRARY lebedevdata HINTS ${lib_hints})
if("${LEBEDEVDATA_LIBRARY}" MATCHES "LEBEDEVDATA_LIBRARY-NOTFOUND")
  message("--   library 'lebedevdata' not found, make sure you have downloaded and build the external libraries!")
else("${LEBEDEVDATA_LIBRARY}" MATCHES "LEBEDEVDATA_LIBRARY-NOTFOUND")
  message("--   found lebedevdata library")
  set(LEBEDEVDATA_LIBRARIES "${LEBEDEVDATA_LIBRARY}")
endif("${LEBEDEVDATA_LIBRARY}" MATCHES "LEBEDEVDATA_LIBRARY-NOTFOUND")

message("-- checking for lebedev_data.hh header")
find_path(LEBEDEVDATA_INCLUDE_DIRS lebedev_data.hh HINTS ${include_hints})
if("${LEBEDEVDATA_INCLUDE_DIRS}" MATCHES "LEBEDEVDATA_INCLUDE_DIRS-NOTFOUND")
  message("--   lebedev_data.hh header not found")
else("${LEBEDEVDATA_INCLUDE_DIRS}" MATCHES "LEBEDEVDATA_INCLUDE_DIRS-NOTFOUND")
  message("--   found lebedev_data.hh header")
  include_sys_dir(${LEBEDEVDATA_INCLUDE_DIRS})
endif("${LEBEDEVDATA_INCLUDE_DIRS}" MATCHES "LEBEDEVDATA_INCLUDE_DIRS-NOTFOUND")

set(LEBEDEVDATA_FOUND 0)
if(NOT "${LEBEDEVDATA_LIBRARY}" MATCHES "LEBEDEVDATA_LIBRARY-NOTFOUND")
  if(NOT "${LEBEDEVDATA_INCLUDE_DIRS}" MATCHES "LEBEDEVDATA_INCLUDE_DIRS-NOTFOUND")
    set(LEBEDEVDATA_FOUND 1)
  endif(NOT "${LEBEDEVDATA_INCLUDE_DIRS}" MATCHES "LEBEDEVDATA_INCLUDE_DIRS-NOTFOUND")
endif(NOT "${LEBEDEVDATA_LIBRARY}" MATCHES "LEBEDEVDATA_LIBRARY-NOTFOUND")

set(HAVE_LEBEDEVDATA ${LEBEDEVDATA_FOUND})

# register all LEBEDEVDATA related flags
if(LEBEDEVDATA_FOUND)
  dune_register_package_flags(LIBRARIES "${LEBEDEVDATA_LIBRARIES}" INCLUDE_DIRS "${LEBEDEVDATA_INCLUDE_DIRS}")
endif()
