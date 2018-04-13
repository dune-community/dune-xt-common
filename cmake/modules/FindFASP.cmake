# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2014, 2016 - 2017)
#   Rene Milk       (2013, 2015, 2018)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# ~~~

message("-- checking for 'fasp' library")
find_library(FASP_LIBRARY fasp "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib/")
if("${FASP_LIBRARY}" MATCHES "FASP_LIBRARY-NOTFOUND")
  message("--   library 'fasp' not found")
else("${FASP_LIBRARY}" MATCHES "FASP_LIBRARY-NOTFOUND")
  message("--   found fasp library")
  set(FASPLIB "${FASP_LIBRARY}")
endif("${FASP_LIBRARY}" MATCHES "FASP_LIBRARY-NOTFOUND")
message("-- checking for 'fasp' header")
find_path(FASP_INCLUDE_DIR fasp.h
          HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../local/include/" "${CMAKE_CURRENT_SOURCE_DIR}/../local/include/fasp/")
if("${FASP_INCLUDE_DIR}" MATCHES "FASP_INCLUDE_DIR-NOTFOUND")
  message("--   'fasp' header not found")
else("${FASP_INCLUDE_DIR}" MATCHES "FASP_INCLUDE_DIR-NOTFOUND")
  message("--   found fasp header")
  include_sys_dir(${FASP_INCLUDE_DIR})
endif("${FASP_INCLUDE_DIR}" MATCHES "FASP_INCLUDE_DIR-NOTFOUND")
if(NOT "${FASP_LIBRARY}" MATCHES "FASP_LIBRARY-NOTFOUND")
  if(NOT "${FASP_INCLUDE_DIR}" MATCHES "FASP_INCLUDE_DIR-NOTFOUND")
    add_definitions(-DHAVE_FASP=1)
  endif(NOT "${FASP_INCLUDE_DIR}" MATCHES "FASP_INCLUDE_DIR-NOTFOUND")
endif(NOT "${FASP_LIBRARY}" MATCHES "FASP_LIBRARY-NOTFOUND")