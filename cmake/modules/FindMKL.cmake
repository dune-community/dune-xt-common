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
find_library(MKL_LP64_LIBRARY mkl_intel_lp64
             HINTS "${CMAKE_SOURCE_DIR}/../local/mkl/lib/intel64/"
                   "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/lib/intel64/"
                   "/opt/intel/mkl/lib/intel64/")
find_library(MKL_SEQUENTIAL_LIBRARY mkl_sequential
             HINTS "${CMAKE_SOURCE_DIR}/../local/mkl/lib/intel64/"
                   "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/lib/intel64/"
                   "/opt/intel/mkl/lib/intel64/")
find_library(MKL_CORE_LIBRARY mkl_core
             HINTS "${CMAKE_SOURCE_DIR}/../local/mkl/lib/intel64/"
                   "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/lib/intel64/"
                   "/opt/intel/mkl/lib/intel64/")

set(MKL_FOUND 0)
if("${MKL_LP64_LIBRARY}" MATCHES "MKL_LP64_LIBRARY-NOTFOUND")
  message("--   library 'mkl_intel_lp64' not found, make sure you have Intel MKL installed")
else("${MKL_LP64_LIBRARY}" MATCHES "MKL_LP64_LIBRARY-NOTFOUND")
  message("--   found mkl_intel_lp64 library")
  if("${MKL_SEQUENTIAL_LIBRARY}" MATCHES "MKL_SEQUENTIAL_LIBRARY-NOTFOUND")
    message("--   library 'mkl_sequential' not found, make sure you have Intel MKL installed")
  else("${MKL_SEQUENTIAL_LIBRARY}" MATCHES "MKL_SEQUENTIAL_LIBRARY-NOTFOUND")
    message("--   found mkl_sequential library")
    if("${MKL_CORE_LIBRARY}" MATCHES "MKL_CORE_LIBRARY-NOTFOUND")
      message("--   library 'mkl_core' not found, make sure you have Intel MKL installed")
    else("${MKL_CORE_LIBRARY}" MATCHES "MKL_CORE_LIBRARY-NOTFOUND")
      message("--   found mkl_core library")
      set(MKL_LIBRARIES "${MKL_LP64_LIBRARY}")
      list(APPEND MKL_LIBRARIES ${MKL_SEQUENTIAL_LIBRARY})
      list(APPEND MKL_LIBRARIES ${MKL_CORE_LIBRARY})
    endif("${MKL_CORE_LIBRARY}" MATCHES "MKL_CORE_LIBRARY-NOTFOUND")
  endif("${MKL_SEQUENTIAL_LIBRARY}" MATCHES "MKL_SEQUENTIAL_LIBRARY-NOTFOUND")
endif("${MKL_LIBRARY}" MATCHES "MKL_LP64_LIBRARY-NOTFOUND")

message("-- checking for mkl.h header")
find_path(MKL_INCLUDE_DIRS mkl.h
          HINTS "${CMAKE_SOURCE_DIR}/../local/mkl/include/"
                "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/include/" "/opt/intel/mkl/include")
if("${MKL_INCLUDE_DIRS}" MATCHES "MKL_INCLUDE_DIRS-NOTFOUND")
  message("--   mkl.h header not found")
else("${MKL_INCLUDE_DIRS}" MATCHES "MKL_INCLUDE_DIRS-NOTFOUND")
  message("--   found mkl.h header")
  include_sys_dir("${MKL_INCLUDE_DIRS}")
endif("${MKL_INCLUDE_DIRS}" MATCHES "MKL_INCLUDE_DIRS-NOTFOUND")

if(NOT "${MKL_LP64_LIBRARY}" MATCHES "MKL_LP64_LIBRARY-NOTFOUND")
  if(NOT "${MKL_SEQUENTIAL_LIBRARY}" MATCHES "MKL_SEQUENTIAL_LIBRARY-NOTFOUND")
    if(NOT "${MKL_CORE_LIBRARY}" MATCHES "MKL_CORE_LIBRARY-NOTFOUND")
      if(NOT "${MKL_INCLUDE_DIRS}" MATCHES "MKL_INCLUDE_DIRS-NOTFOUND")
        set(MKL_FOUND 1)
      endif(NOT "${MKL_INCLUDE_DIRS}" MATCHES "MKL_INCLUDE_DIRS-NOTFOUND")
    endif(NOT "${MKL_CORE_LIBRARY}" MATCHES "MKL_CORE_LIBRARY-NOTFOUND")
  endif(NOT "${MKL_SEQUENTIAL_LIBRARY}" MATCHES "MKL_SEQUENTIAL_LIBRARY-NOTFOUND")
endif(NOT "${MKL_LP64_LIBRARY}" MATCHES "MKL_LP64_LIBRARY-NOTFOUND")

set(HAVE_MKL ${MKL_FOUND})

# register all MKL related flags
if(MKL_FOUND)
  dune_register_package_flags(LIBRARIES "${MKL_LIBRARIES}" INCLUDE_DIRS "${MKL_INCLUDE_DIRS}")
endif()
