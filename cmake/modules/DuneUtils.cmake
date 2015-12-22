# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2015)
#   Rene Milk       (2010 - 2015)
#   Sven Kaulmann   (2013)

include(CheckCXXSourceCompiles)


include(DuneStuffMacros)

set(DUNE_STUFF_ROOT ${dune-stuff_SOURCE_DIR}/
		CACHE STRING "path to dune-stuff module root" )
set(DUNE_STUFF_TEST_DIR ${DUNE_STUFF_ROOT}/dune/stuff/test 
		CACHE STRING "path to dir containing gtest subdir")
mark_as_advanced(DUNE_STUFF_ROOT DUNE_STUFF_TEST_DIR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DUNE_STUFF_ROOT}/cmake/modules)

function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE ${NEW_LIST_SPACE} ${ITEM})
  endforeach()
#   string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

MACRO( DS_HEADERCHECK )
	ADD_CUSTOM_TARGET( ds_headercheck SOURCES ${ARGN} )
	FOREACH( HEADER ${ARGN} )
		string(REPLACE "/" "_" fn ${HEADER})
		SET( TEST_NAME "ds_headercheck_${fn}")
		configure_file( ${DUNE_STUFF_ROOT}/cmake/headercheck.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/headercheck/${TEST_NAME}.cpp )
		ADD_LIBRARY( ${TEST_NAME} EXCLUDE_FROM_ALL STATIC ${CMAKE_CURRENT_BINARY_DIR}/headercheck/${TEST_NAME}.cpp )
		set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX)
		add_dependencies( ds_headercheck ${TEST_NAME} )
	ENDFOREACH( HEADER )
ENDMACRO( HEADERCHECK )

MACRO( DEPENDENCYCHECK )
	ADD_CUSTOM_TARGET( dependenycheck SOURCES ${ARGN} )
	FOREACH( HEADER ${ARGN} )
		string(REPLACE "/" "_" fn ${HEADER})
		SET( TEST_NAME "dependenycheck_${fn}")
		TO_LIST_SPACES( CMAKE_CXX_FLAGS TEST_NAME_FLAGS )
		SET(XARGS ${TEST_NAME_FLAGS} -DHAVE_CONFIG_H -H -c ${HEADER} -w)
		ADD_CUSTOM_TARGET( ${TEST_NAME} + ${DUNE_STUFF_ROOT}/cmake/dependencyinfo.py ${CMAKE_CXX_COMPILER} ${XARGS} ${CMAKE_CURRENT_SOURCE_DIR} ${fn}.dep)
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

MACRO( ADD_DUNE_MODULES )
	FOREACH( MODULE ${ARGN} )
        if (EXISTS "${DUNE_STUFF_ROOT}/../dune-${MODULE}/config.h")
            INCLUDE_SYS_DIR( ${DUNE_STUFF_ROOT}/../dune-${MODULE} )
            list( APPEND DUNE_MODULE_DIRS ${DUNE_STUFF_ROOT}/../dune-${MODULE})
            set( ${MODULE}_LIBDIR ${DUNE_STUFF_ROOT}/../dune-${MODULE}/lib/.libs)
            LINK_DIRECTORIES( ${${MODULE}_LIBDIR} )
            SET( LIBNAME libdune${MODULE}.a )
            FILE( GLOB_RECURSE ${MODULE}_HEADER "${DUNE_STUFF_ROOT}/../dune-${MODULE}/dune/*.hh" )
            LIST( APPEND DUNE_HEADERS ${${MODULE}_HEADER} )
            find_library( ${MODULE}_FOUND ${LIBNAME} PATHS ${${MODULE}_LIBDIR})
            if ( ${MODULE}_FOUND )
                string(TOUPPER ${MODULE} UPMODULE )
                LIST( APPEND DUNE_LIBS ${${MODULE}_FOUND} )
                ADD_DEFINITIONS( -DHAVE_DUNE_${UPMODULE} )
                MESSAGE(STATUS "found dune-${MODULE}")
            else()
                MESSAGE(STATUS "found dune-${MODULE}, but did not find ${LIBNAME}")
            endif()
        else(EXISTS "${DUNE_STUFF_ROOT}/../dune-${MODULE}/config.h")
            MESSAGE(STATUS "did not find dune-${MODULE}")
        endif(EXISTS "${DUNE_STUFF_ROOT}/../dune-${MODULE}/config.h")
	ENDFOREACH(MODULE)
ENDMACRO( ADD_DUNE_MODULES )

MACRO( ADD_MY_MODULES )
	FOREACH( MODULE ${ARGN} )
		INCLUDE_DIR( ${DUNE_STUFF_ROOT}/../dune-${MODULE} )
		LINK_DIRECTORIES(${DUNE_STUFF_ROOT}/../dune-${MODULE}/${MODULE}/.libs )
		FILE( GLOB_RECURSE ${MODULE}_HEADER "${DUNE_STUFF_ROOT}/../dune-${MODULE}/dune/*.hh" )
		LIST( APPEND DUNE_HEADERS ${${MODULE}_HEADER} )
	ENDFOREACH(MODULE)
ENDMACRO( ADD_MY_MODULES )

MACRO( SET_CONFIGHEADER_VARS )
	IF( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
		SET( ALUGRID_FOUND "1" )
	ELSE( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
		SET( ALUGRID_FOUND "0" )
	ENDIF( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
ENDMACRO( SET_CONFIGHEADER_VARS )

set( DUNE_TEST_TIMEOUT 180 CACHE STRING "per-test timeout in seconds")

include(CTest)
macro(BEGIN_TESTCASES)
# https://cmake.org/cmake/help/v3.0/module/FindGTest.html http://purplekarrot.net/blog/cmake-and-test-suites.html

	file( GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )
	foreach( source ${test_sources} )
		get_filename_component(testbase ${source} NAME_WE)
                if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
		    set( inifile ${testbase}.mini)
    		    dune_add_system_test(SOURCE ${testbase}.cc
                                         INIFILE ${inifile}
                                         BASENAME test_${testbase}
                                         CREATED_TARGETS targetlist_${testbase}
                                         SCRIPT ${DUNE_STUFF_ROOT}/scripts/dune_execute.py
                                         ${DEBUG_MACRO_TESTS})
		foreach(testname ${targetlist_${testbase}})
    		    target_link_libraries( ${testname} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_stuff )
		    list(APPEND testnames ${testname} )
                endforeach(testname)
                else( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testbase}.mini )
                    add_executable( test_${testbase} ${source} ${COMMON_HEADER} )
                    target_link_libraries( test_${testbase} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_stuff )
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
    foreach( test ${testnames} )
      add_dune_mpi_flags(${test})
      if(dune-grid_FOUND)
        add_dune_alberta_flags(${test})
        add_dune_alugrid_flags(${test})
        add_dune_superlu_flags(${test})
        add_dune_ug_flags(${test})
      endif(dune-grid_FOUND)
      if(COMMAND add_dune_tbb_flags AND TBB_FOUND)
        add_dune_tbb_flags(${test})
      endif()
     endforeach( test ${testnames} )
	add_custom_target(test_binaries DEPENDS ${testnames})
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DUNE_TEST_TIMEOUT}
                  DEPENDS test_binaries)
endmacro(END_TESTCASES)

add_custom_target( config_refresh
				${CMAKE_CURRENT_SOURCE_DIR}/cmake/regen_config_header.sh ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h
				)

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

ENABLE_TESTING()
