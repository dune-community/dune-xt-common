# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2013 - 2014, 2016 - 2017)
#   René Fritze     (2013 - 2016, 2018)
#   Sven Kaulmann   (2014)
#   Tobias Leibner  (2016, 2018)
# ~~~

# enables "IN_LIST operator
cmake_policy(SET CMP0057 NEW)

include(XtCompilerSupport)
include(XtTooling)

# library checks  #########################################################################
set(DS_REQUIRED_BOOST_LIBS atomic chrono date_time filesystem system thread timer)
find_package(PkgConfig)
find_package(Boost 1.48.0 COMPONENTS ${DS_REQUIRED_BOOST_LIBS} REQUIRED)
dune_register_package_flags(INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
foreach(_boost_lib ${DS_REQUIRED_BOOST_LIBS})
  set(_BOOST_LIB "")
  string(TOUPPER "${_boost_lib}" _BOOST_LIB)
  dune_register_package_flags(LIBRARIES ${Boost_${_BOOST_LIB}_LIBRARY})
  if(TARGET Boost::${_boost_lib})
    dune_register_package_flags(LIBRARIES Boost::${_boost_lib})
  endif()
endforeach(_boost_lib ${DS_REQUIRED_BOOST_LIBS})

find_package(Eigen3 3.2.0)
if(EIGEN3_FOUND)
  dune_register_package_flags(INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR} COMPILE_DEFINITIONS "ENABLE_EIGEN=1")
  set(HAVE_EIGEN 1)
else(EIGEN3_FOUND)
  dune_register_package_flags(COMPILE_DEFINITIONS "ENABLE_EIGEN=0")
  set(HAVE_EIGEN 0)
endif(EIGEN3_FOUND)

# intel mic and likwid don't mix
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "k1om")
  include(FindLIKWID)
  find_package(LIKWID)
  if(LIKWID_FOUND)
    dune_register_package_flags(INCLUDE_DIRS ${LIKWID_INCLUDE_DIR} LIBRARIES ${LIKWID_LIBRARY})
  endif(LIKWID_FOUND)
else()
  set(HAVE_LIKWID 0)
endif()

include(DuneTBB)

if(HAVE_MPI)
  include(FindMPI4PY)
  if(MPI4PY_FOUND)
    # this only works in dependent modules
    dune_register_package_flags(INCLUDE_DIRS "${MPI4PY_INCLUDE_DIR}")
    # this only works in dune-xt-common itself
    include_directories("${MPI4PY_INCLUDE_DIR}")
  else()
    message(FATAL_ERROR "MPI enabled builds need mpi4py too")
  endif()
endif()
# end library checks  #####################################################################

# misc vars  #########################################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(DS_MAX_MIC_THREADS CACHE INTEGER 120)
set(DUNE_XT_COMMON_TEST_DIR ${dune-xt-common_SOURCE_DIR}/dune/xt/common/test)
set(ENABLE_PERFMON 0 CACHE STRING "enable likwid performance monitoring API usage")
if(NOT DS_HEADERCHECK_DISABLE)
  set(ENABLE_HEADERCHECK 1)
endif(NOT DS_HEADERCHECK_DISABLE)
set(DXT_TEST_TIMEOUT 180 CACHE STRING "per-test timeout in seconds")
set(DXT_TEST_PROCS 1 CACHE STRING "run N tests in parallel")
