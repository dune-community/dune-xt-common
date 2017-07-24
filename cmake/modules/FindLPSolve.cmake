# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Tobias Leibner (2017)

message("-- checking for 'lpsolve' library")
find_library(LPSOLVE_LIBRARY NAMES lpsolve lpsolve55 HINTS "${CMAKE_SOURCE_DIR}/../local/lib/" PATHS "/usr/lib/" "/usr/local/lib/")
if("${LPSOLVE_LIBRARY}" MATCHES "LPSOLVE_LIBRARY-NOTFOUND")
	message("--   library 'lpsolve' not found")
else("${LPSOLVE_LIBRARY}" MATCHES "LPSOLVE_LIBRARY-NOTFOUND")
  message("--   found lpsolve library")
  set(LPSolve_LIBRARIES "${LPSOLVE_LIBRARY}")
endif("${LPSOLVE_LIBRARY}" MATCHES "LPSOLVE_LIBRARY-NOTFOUND")

message("-- checking for lpsolve/lp_lib.h header")
find_path(LPSolve_INCLUDE_DIRS lpsolve/lp_lib.h HINTS "${CMAKE_SOURCE_DIR}/../local/include/")
if("${LPSolve_INCLUDE_DIRS}" MATCHES "LPSolve_INCLUDE_DIRS-NOTFOUND")
  message("--   lpsolve header not found")
else("${LPSolve_INCLUDE_DIRS}" MATCHES "LPSolve_INCLUDE_DIRS-NOTFOUND")
  message("--   found lpsolve header")
  include_sys_dir(${LPSolve_INCLUDE_DIRS})
endif("${LPSolve_INCLUDE_DIRS}" MATCHES "LPSolve_INCLUDE_DIRS-NOTFOUND")

set(LPSolve_FOUND 0)
if(NOT "${LPSOLVE_LIBRARY}" MATCHES "LPSOLVE_LIBRARY-NOTFOUND")
  if(NOT "${LPSolve_INCLUDE_DIRS}" MATCHES "LPSolve_INCLUDE_DIRS-NOTFOUND")
    set(LPSolve_FOUND 1)
  endif(NOT "${LPSolve_INCLUDE_DIRS}" MATCHES "LPSolve_INCLUDE_DIRS-NOTFOUND")
endif(NOT "${LPSOLVE_LIBRARY}" MATCHES "LPSOLVE_LIBRARY-NOTFOUND")

set(HAVE_LPSOLVE ${LPSolve_FOUND})

# register all LPSolve related flags
if(LPSolve_FOUND)
  dune_register_package_flags(
    LIBRARIES "${LPSolve_LIBRARIES}"
    INCLUDE_DIRS "${LPSolve_INCLUDE_DIRS}")
endif()
