# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2012 - 2017)
#   Rene Milk       (2010 - 2016)
#   Sven Kaulmann   (2013)
#   Tobias Leibner  (2016)

project("dune-xt-common" CXX)

# local environment
set( ENV{PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/bin:$ENV{PATH}" )
set( ENV{LD_LIBRARY_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib:$ENV{LD_LIBRARY_PATH}" )
set( ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )

# cmake specific
cmake_minimum_required( VERSION 3.1 )

set( CMAKE_COLOR_MAKEFILE ON)

# guess dune-common build dir
if(NOT (dune-common_DIR OR dune-common_ROOT OR
        "${CMAKE_PREFIX_PATH}" MATCHES ".*dune-common.*"))
    string(REPLACE  ${CMAKE_PROJECT_NAME} dune-common dune-common_DIR
    ${PROJECT_BINARY_DIR})
endif()

#find dune-common and set the module path
find_package(dune-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${dune-common_MODULE_PATH}
"${PROJECT_SOURCE_DIR}/cmake/modules")

#include the dune macros
include(DuneMacros)

# start a dune project with information from dune.module
dune_project()

# dune-xt-common cmake includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
execute_process(
    COMMAND ${CMAKE_SOURCE_DIR}/getRevision.sh
    OUTPUT_VARIABLE COMMIT
    ERROR_VARIABLE shell_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

SET( DS_FLAGS_Release
  "-fPIC"
  CACHE STRING "")
set(DS_FLAGS_RelWithDebInfo
  "-fPIC"
  CACHE STRING "")
SET( DS_FLAGS_Debug
  "-O0" "-g3" "-ggdb" "-Wunused-variable" "-fno-strict-aliasing" "-fPIC" "-Wall" "-Wextra" "-Wparentheses" "-pedantic"  "-Wshadow" "-Wundef" "-Wnon-virtual-dtor"
  CACHE STRING "")

#aka anything but intel, but cmake has identifier for icc
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list( APPEND DS_FLAGS_Debug
         -Wc++0x-compat -Wredundant-decls -Winline  -ftemplate-backtrace-limit=0
      )
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # for us effectively icc
  list( APPEND DS_FLAGS_Debug
        -Wcast-qual -Wformat=2 -Winit-self -Woverloaded-virtual -Wshadow -Wsign-conversion -Wsign-promo -Wundef  -Wno-unused)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list( APPEND DS_FLAGS_Debug
        -Wnon-literal-null-conversion -Wused-but-marked-unused -Wno-tautological-compare -Wfloat-equal -Wdisabled-macro-expansion -Wcovered-switch-default -Wswitch-enum -Wunreachable-code -Wshorten-64-to-32
      )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list( APPEND DS_FLAGS_Debug
        -Wlogical-op  -Og
      )
endif()

dune_enable_all_packages(INCLUDE_DIRS ${dune-xt-common_SOURCE_DIR}/dune
                         COMPILE_DEFINITIONS DNDEBUG DDNDEBUG
                         COMPILE_OPTIONS ${DS_FLAGS_${CMAKE_BUILD_TYPE}}
                         MODULE_LIBRARIES dunextcommon gtest_dune_xt_common
                         )

include(DuneUtils)
include(GridUtils)

add_header_listing()
make_dependent_modules_sys_included()
if (EIGEN3_FOUND)
  add_definitions("-isystem ${EIGEN3_INCLUDE_DIR}")
endif (EIGEN3_FOUND)

add_subdirectory(dune)
add_subdirectory("cmake/modules")
add_subdirectory(doc/doxygen)

dune_install_python_package(PATH python)
if(dune-pybindxi_FOUND)
  foreach(_file
          dune/__init__.py
          dune/xt/__init__.py
          dune/xt/common/__init__.py
         )
  execute_process(COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/python/${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file})
  endforeach()
endif()


add_format(${CMAKE_CURRENT_SOURCE_DIR})
add_tidy(${CMAKE_CURRENT_SOURCE_DIR})

finalize_dune_project(GENERATE_CONFIG_H_CMAKE)

# patch config.h to remove warnings about unused HAVE_GRIDTYPE
execute_process(COMMAND sed -i -e "s,^\\([ ]*\\)\#if HAVE_GRIDTYPE$,\\1\#if defined(HAVE_GRIDTYPE) \\&\\& HAVE_GRIDTYPE,g" ${CMAKE_CURRENT_BINARY_DIR}/config.h )
