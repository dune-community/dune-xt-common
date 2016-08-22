# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2016)
#   Rene Milk       (2010 - 2016)
#   Sven Kaulmann   (2013)
#   Tobias Leibner  (2015)

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

enable_testing()

macro(BEGIN_TESTCASES)
# https://cmake.org/cmake/help/v3.0/module/FindGTest.html http://purplekarrot.net/blog/cmake-and-test-suites.html
	file( GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )
        if ( DEFINED dune-xt-common_DIR )
            set( dune-xt-common-path ${dune-xt-common_DIR} )
        else ( DEFINED dune-xt-common_DIR )
            set( dune-xt-common-path ${dune-xt-common_SOURCE_DIR} )
        endif ( DEFINED dune-xt-common_DIR )
	foreach( source ${test_sources} )
		get_filename_component(testbase ${source} NAME_WE)
                if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
		    set( inifile ${testbase}.mini)
    		    dune_add_system_test(SOURCE ${testbase}.cc
                                         INIFILE ${inifile}
                                         BASENAME test_${testbase}
                                         CREATED_TARGETS targetlist_${testbase}
                                         SCRIPT ${dune-xt-common-path}/bin/dune_execute.py
                                         ${DEBUG_MACRO_TESTS})
                    foreach(testname ${targetlist_${testbase}})
                        target_link_libraries( ${testname} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common )
                        list(APPEND dxt_test_binaries ${testname} )
                    endforeach(testname)
                else( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
                    add_executable( test_${testbase} ${source} ${COMMON_HEADER} )
                    target_link_libraries( test_${testbase} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common )
                    add_test( NAME test_${testbase} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testbase}
                                          --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/test_${testbase}.xml )
                    list(APPEND dxt_test_binaries test_${testbase} )
	        endif( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
        endforeach( source )
endmacro(BEGIN_TESTCASES)

macro(END_TESTCASES)
    # this ensures binaries are build prior to "make test"
    add_directory_test_target(_test_target)
    add_dependencies(${_test_target} ${dxt_test_binaries})
    
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
                        DEPENDS test_binaries)
    add_custom_target(recheck COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DXT_TEST_TIMEOUT} --rerun-failed -j ${DXT_TEST_PROCS}
                        DEPENDS test_binaries)
endmacro(END_TESTCASES)

ENABLE_TESTING()
