# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2017)
#   Rene Milk       (2010 - 2018)
#   Sven Kaulmann   (2013)
#   Tobias Leibner  (2015 - 2017)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)

include(CheckCXXSourceCompiles)
include(DuneXtCommonMacros)
include(CTest)
include(DuneXTInstallPythonPackage)

function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE ${NEW_LIST_SPACE} ${ITEM})
  endforeach()
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

if ( DEFINED dune-xt-common_DIR )
    set( dune-xt-common-path ${dune-xt-common_DIR} )
else ( DEFINED dune-xt-common_DIR )
    set( dune-xt-common-path ${dune-xt-common_SOURCE_DIR} )
endif ( DEFINED dune-xt-common_DIR )
if ( DEFINED dune-xt-common_MODULE_PATH )
    # dependent modules
    set( dune-xt-common-module-path ${dune-xt-common_MODULE_PATH} )
else ( DEFINED dune-xt-common_MODULE_PATH )
    # xt-common itself
    set( dune-xt-common-module-path ${PROJECT_SOURCE_DIR}/cmake/modules )
endif ( DEFINED dune-xt-common_MODULE_PATH )

enable_testing()

find_package(Qhull)
find_package(LPSolve)
find_package(MKL)
find_package(LAPACKE)
find_package(CBLAS)
find_package(MatExp)
find_package(Fekete)
find_package(LebedevData)

include(DuneXTTesting)

macro(add_header_listing)
    # header
    file( GLOB_RECURSE xtcommon "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.pbh")
    set( COMMON_HEADER ${xtcommon} ${DUNE_HEADERS} )

    # add header of dependent modules for header listing
    foreach(_mod ${ALL_DEPENDENCIES})
        file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/../${_mod}/*.hh" "${CMAKE_CURRENT_SOURCE_DIR}/../${_mod}/*.pbh")
        list(APPEND COMMON_HEADER ${HEADER_LIST})
    endforeach(_mod DEPENDENCIES)
    set_source_files_properties(${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1)
    add_custom_target(common_header SOURCES ${COMMON_HEADER})
endmacro(add_header_listing)

macro(make_dependent_modules_sys_included)
    # disable most warnings from dependent modules
    foreach(_mod ${ALL_DEPENDENCIES})
        if(${_mod}_INCLUDE_DIRS)
            foreach( _idir ${${_mod}_INCLUDE_DIRS} )
                INCLUDE_SYS_DIR(${_idir})
            endforeach( _idir )
        endif(${_mod}_INCLUDE_DIRS)
    endforeach(_mod DEPENDENCIES)
endmacro(make_dependent_modules_sys_included)

macro(add_pylicense)
    file(GLOB configs ${PROJECT_SOURCE_DIR}/.pylicense*.cfg )
    foreach(cfg ${configs})
        string(REGEX REPLACE ".*/([^/]*)" "\\1" cfg_target ${cfg})
        string(REPLACE ${PROJECT_SOURCE_DIR} "" cfg_target ${cfg_target})
        string(REGEX REPLACE "(.*)/[^/]*" "\\1" cfg_target ${cfg_target})
        string(REGEX REPLACE "/" "_" cfg_target ${cfg_target})
        list(APPEND cfg_targets ${cfg_target})
        add_custom_target(${cfg_target} ${CMAKE_BINARY_DIR}/dune-env pylicense
                        "--cfg=${cfg}" "${PROJECT_SOURCE_DIR}"
                        VERBATIM USES_TERMINAL)
    endforeach(cfg ${configs})
    add_custom_target(license DEPENDS ${cfg_targets})
endmacro(add_pylicense)
