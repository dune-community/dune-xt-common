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

MACRO( DEPENDENCYCHECK )
	ADD_CUSTOM_TARGET( dependenycheck SOURCES ${ARGN} )
	FOREACH( HEADER ${ARGN} )
		string(REPLACE "/" "_" fn ${HEADER})
		SET( TEST_NAME "dependenycheck_${fn}")
		TO_LIST_SPACES( CMAKE_CXX_FLAGS TEST_NAME_FLAGS )
		SET(XARGS ${TEST_NAME_FLAGS} -DHAVE_CONFIG_H -H -c ${HEADER} -w)
		ADD_CUSTOM_TARGET( ${TEST_NAME} + ${dune-xt-common_SOURCE_DIR}/cmake/dependencyinfo.py ${CMAKE_CXX_COMPILER} ${XARGS} ${CMAKE_CURRENT_SOURCE_DIR} ${fn}.dep)
		add_dependencies( dependenycheck ${TEST_NAME} )
	ENDFOREACH( HEADER )
ENDMACRO( DEPENDENCYCHECK )

MACRO( ADD_CPPCHECK )
	find_program( CPPCHECK_BINARY NAMES cppcheck )
	IF( EXISTS ${CPPCHECK_BINARY} )
		SET(CPPINLINST ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.files )
		IF( EXISTS ${CPPINLINST} )
			file(REMOVE  ${CPPINLINST} )
		ENDIF()
		FOREACH( SOURCEFILE ${ARGN} )
			FILE( APPEND  ${CPPINLINST}  "${SOURCEFILE}\n" )
		ENDFOREACH( SOURCEFILE )
		TO_LIST_SPACES( CPPCHECK_FLAGS_SPLIT ${CMAKE_CXX_FLAGS} )
		ADD_CUSTOM_TARGET(  cppcheck cppcheck --enable=all --xml --report-progress --file-list=${CPPINLINST}
				${CPPCHECK_FLAGS_SPLIT}  2>cppcheck.xml )
	ELSE( EXISTS ${CPPCHECK_BINARY} )
		MESSAGE( STATUS "Not adding cppcheck target because cppcheck executable not found" )
	ENDIF( EXISTS ${CPPCHECK_BINARY} )
ENDMACRO( ADD_CPPCHECK )

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
		    list(APPEND testnames ${testname} )
                endforeach(testname)
                else( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
                    add_executable( test_${testbase} ${source} ${COMMON_HEADER} )
                    target_link_libraries( test_${testbase} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common )
                    add_test( NAME test_${testbase} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testbase}
                                          --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/test_${testbase}.xml )
                    # currently property seems to have no effect
                    set_tests_properties(test_${testbase} PROPERTIES TIMEOUT ${DUNE_TEST_TIMEOUT})
                    list(APPEND testnames test_${testbase} )
	        endif( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
        endforeach( source )
endmacro(BEGIN_TESTCASES)

macro(END_TESTCASES)
    add_directory_test_target(_test_target)
    add_dependencies(${_test_target} ${testnames})
	add_custom_target(test_binaries DEPENDS ${testnames})
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DUNE_TEST_TIMEOUT}
                  DEPENDS test_binaries)
endmacro(END_TESTCASES)

ENABLE_TESTING()
