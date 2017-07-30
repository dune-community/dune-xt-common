# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2012 - 2017)
#   Rene Milk       (2010 - 2017)
#   Sven Kaulmann   (2013)
#   Tobias Leibner  (2015 - 2017)

include(CheckCXXSourceCompiles)
include(DuneXtCommonMacros)
include(CTest)

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

macro(get_headercheck_targets)
# this is mostly c&p from dune-common, since we need a way to extract
# all target names to pass to our load balancing script
  if(ENABLE_HEADERCHECK)
    get_property(headerlist GLOBAL PROPERTY headercheck_list)
    foreach(header ${headerlist})
      #do some name conversion
      string(REGEX REPLACE ".*/([^/]*)" "\\1" simple ${header})
      string(REPLACE ${PROJECT_SOURCE_DIR} "" rel ${header})
      string(REGEX REPLACE "(.*)/[^/]*" "\\1" relpath ${rel})
      string(REGEX REPLACE "/" "_" targname ${rel})
      list(APPEND dxt_headercheck_targets "headercheck_${targname}")
    endforeach(header ${headerlist})
  endif(ENABLE_HEADERCHECK)
endmacro(get_headercheck_targets)

find_package(Qhull)
find_package(LPSolve)
find_package(LAPACKE)
find_package(MatExp)
find_package(LebedevData)
macro(BEGIN_TESTCASES)
# https://cmake.org/cmake/help/v3.0/module/FindGTest.html http://purplekarrot.net/blog/cmake-and-test-suites.html
	file( GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )
	foreach( source ${test_sources} )
		get_filename_component(testbase ${source} NAME_WE)
		string(REPLACE ".cc" ".mini" minifile ${source})
                if( EXISTS ${minifile} )
                    dune_add_system_test(SOURCE ${source} ${COMMON_HEADER}
                                         INIFILE ${minifile}
                                         BASENAME test_${testbase}
                                         CREATED_TARGETS targetlist_${testbase}
                                         SCRIPT dune_xt_execute.py
                                         ${DEBUG_MACRO_TESTS})
                    foreach(target ${targetlist_${testbase}})
                        target_link_libraries( ${target} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common )
                        list(APPEND dxt_test_binaries ${target} )
			#                        set(dxt_test_names_${target} ${testlist_${testbase}_${target}})
                    endforeach(target)
                else( EXISTS ${minifile} )
                    set(target test_${testbase})
		    dune_add_test( NAME ${target}
			           SOURCES ${source} ${COMMON_HEADER}
				   LINK_LIBRARIES ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common
				   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target}
                                          --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${target}.xml )
                    list(APPEND dxt_test_binaries ${target} )
                    set(dxt_test_names_${target} ${target})
	        endif( EXISTS ${minifile} )
        endforeach( source )
        file( GLOB_RECURSE test_templates "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
        foreach( template ${test_templates} )
            get_filename_component(testbase ${template} NAME_WE)
            string(REPLACE ".tpl" ".py" config_fn "${template}")
            string(REPLACE ".tpl" ".tpl.cc" out_fn "${template}")
            string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" out_fn "${out_fn}")
            add_custom_command(OUTPUT ${out_fn}
                               COMMAND ${CMAKE_BINARY_DIR}/dune-env dxt_code_generation.py
                                      "${config_fn}" "${template}"  "${CMAKE_BINARY_DIR}" "${out_fn}"
                               DEPENDS "${config_fn}" "${template}" "${CMAKE_CURRENT_SOURCE_DIR}/spaces.py"
                               VERBATIM USES_TERMINAL)
            set(target test_${testbase})
            dune_add_test( NAME ${target}
                           SOURCES ${out_fn} ${COMMON_HEADER}
                           LINK_LIBRARIES ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common
                           COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target}
                                    --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${target}.xml )
            list(APPEND dxt_test_binaries ${target} )
            set(dxt_test_names_${target} ${target})
        endforeach( source )
        add_custom_target(test_templates SOURCES ${test_templates})
endmacro(BEGIN_TESTCASES)

macro(END_TESTCASES)
    # this excludes meta-ini variation test cases because
    # there binary name != test name
    foreach( test ${dxt_test_binaries} )
        if (TARGET test)
            set_tests_properties(${test} PROPERTIES TIMEOUT ${DXT_TEST_TIMEOUT})
        endif(TARGET test)
    endforeach( test ${dxt_test_binaries} )

    add_custom_target(test_binaries DEPENDS ${dxt_test_binaries})
#     add_dependencies(test test_binaries)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DXT_TEST_TIMEOUT} -j ${DXT_TEST_PROCS}
                        DEPENDS test_binaries USES_TERMINAL)
    add_custom_target(recheck COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DXT_TEST_TIMEOUT} --rerun-failed -j ${DXT_TEST_PROCS}
                        DEPENDS test_binaries USES_TERMINAL)
    foreach (target ${dxt_test_binaries})
        set(all_sorted_testnames "${all_sorted_testnames}/${dxt_test_names_${target}}")
    endforeach (target ${dxt_test_binaries})
    set(dxt_headercheck_targets "")
    get_headercheck_targets(dxt_headercheck_targets)
    configure_file(${dune-xt-common-module-path}/dxt_test_binaries.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/dxt_test_binaries.cmake)
    configure_file(${dune-xt-common-module-path}/dxt_all_sorted_testnames.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/dxt_all_sorted_testnames.cmake)
    configure_file(${dune-xt-common-module-path}/dxt_headercheck_targets.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/dxt_headercheck_targets.cmake)
    add_custom_target(rerun_test_distribution ${CMAKE_BINARY_DIR}/dune-env distribute_testing.py
                        "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
                        "${CMAKE_CURRENT_BINARY_DIR}/dxt_test_binaries.cmake"
                        "${CMAKE_CURRENT_BINARY_DIR}/dxt_all_sorted_testnames.cmake"
                        "${CMAKE_CURRENT_BINARY_DIR}/dxt_headercheck_targets.cmake"
                        "${DXT_BIN_COUNT}"
                        VERBATIM USES_TERMINAL)
    add_custom_target(copy_builders_if_different
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/builder_definitions.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/builder_definitions.cmake")
    add_custom_target(refresh_test_timings)
    add_dependencies(copy_builders_if_different rerun_test_distribution)
    add_dependencies(refresh_test_timings copy_builders_if_different)
endmacro(END_TESTCASES)

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
    #disable most warnings from dependent modules
    foreach(_mod ${ALL_DEPENDENCIES})
        if(${_mod}_INCLUDE_DIRS)
            foreach( _idir ${${_mod}_INCLUDE_DIRS} )
                add_definitions("-isystem ${_idir}")
            endforeach( _idir )
        endif(${_mod}_INCLUDE_DIRS)
    endforeach(_mod DEPENDENCIES)
endmacro(make_dependent_modules_sys_included)
