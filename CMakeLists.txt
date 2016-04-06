# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2016)
#   Rene Milk       (2010 - 2016)
#   Sven Kaulmann   (2013)

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

SET( DS_FLAGS_Release CACHE STRING
  "-O2 -fPIC ${CMAKE_CXX_FLAGS_RELEASE}")
set(DS_FLAGS_RelWithDebInfo "-g ${DS_FLAGS_Release}")

SET( DS_FLAGS_Debug CACHE STRING
  "-O0 -g3 -ggdb -Wunused-variable -fno-strict-aliasing -fPIC -Wall -Wextra -Wparentheses -pedantic  -Wshadow -Wundef -Wnon-virtual-dtor")

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

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(DS_COMPILE_OPTIONS ${DS_FLAGS_Debug})
else(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(DS_COMPILE_OPTIONS ${DS_FLAGS_Release})
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

dune_enable_all_packages(INCLUDE_DIRS ${dune-xt-common_SOURCE_DIR}/dune
                         COMPILE_DEFINITIONS DNDEBUG DDNDEBUG
                         COMPILE_OPTIONS ${DS_FLAGS_${CMAKE_BUILD_TYPE}}
                         MODULE_LIBRARIES dunextcommon gtest_dune_xt_common
                         VERBOSE
                         )

include(DuneUtils)
include(GridUtils)

# header
file( GLOB_RECURSE xtcommon "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )
set( COMMON_HEADER ${xtcommon} ${DUNE_HEADERS} )

# add header of dependent modules for header listing
foreach(_mod ${ALL_DEPENDENCIES})
file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/../${_mod}/*.hh")
list(APPEND COMMON_HEADER ${HEADER_LIST})
endforeach(_mod DEPENDENCIES)
set_source_files_properties(${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1)

# add symlink for dune_execute.py to build-directory to be able to access it from other (xt-) modules
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
dune_symlink_to_source_files(FILES bin/dune_execute.py)

#disable most warnings from dependent modules
foreach(_mod ${ALL_DEPENDENCIES})
    dune_module_to_uppercase(_upper_case "${_mod}")
    if(${_mod}_INCLUDE_DIRS)
        foreach( _idir ${${_mod}_INCLUDE_DIRS} )
            add_definitions("-isystem ${_idir}")
        endforeach( _idir )
    endif(${_mod}_INCLUDE_DIRS)
endforeach(_mod DEPENDENCIES)

add_subdirectory(dune)
add_subdirectory("cmake/modules")
add_subdirectory(doc/doxygen)

finalize_dune_project(GENERATE_CONFIG_H_CMAKE)
