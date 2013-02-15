include(CheckCXXSourceCompiles)

set(DUNE_STUFF_ROOT ${PROJECT_SOURCE_DIR}/../dune-stuff 
		CACHE STRING "path to dune-stuff module root" )
set(DUNE_STUFF_TEST_DIR ${DUNE_STUFF_ROOT}/dune/stuff/test 
		CACHE STRING "path to dir containing gtest subdir")
mark_as_advanced(DUNE_STUFF_ROOT DUNE_STUFF_TEST_DIR)

function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE ${NEW_LIST_SPACE} ${ITEM})
  endforeach()
#   string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

MACRO(INCLUDE_DIR)
	FOREACH( ARG ${ARGN} )
		IF(IS_DIRECTORY ${ARG} )
			INCLUDE_DIRECTORIES(${ARG})
		ELSE(IS_DIRECTORY ${ARG} )
			MESSAGE( STATUS "Include directory ${ARG} does not exist" )
		ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_DIR)

MACRO(INCLUDE_SYS_DIR)
	FOREACH( ARG ${ARGN} )
		IF(IS_DIRECTORY ${ARG} )
			ADD_DEFINITIONS("-isystem${ARG}")
			INCLUDE_DIRECTORIES(${ARG})
		ELSE(IS_DIRECTORY ${ARG} )
			MESSAGE( STATUS "Include directory ${ARG} does not exist" )
		ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_SYS_DIR)

MACRO( HEADERCHECK )
	ADD_CUSTOM_TARGET( headercheck SOURCES ${ARGN} )
	FOREACH( HEADER ${ARGN} )
		string(REPLACE "/" "_" fn ${HEADER})
		SET( TEST_NAME "headercheck_${fn}")
		configure_file( ${DUNE_STUFF_ROOT}/cmake/headercheck.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/headercheck/${TEST_NAME}.cpp )
		ADD_LIBRARY( ${TEST_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/headercheck/${TEST_NAME}.cpp )
		set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX)
		add_dependencies( headercheck ${TEST_NAME} )
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
		else()
			MESSAGE(STATUS "did not find library ${LIBNAME}: did not add dune-${MODULE}")
		endif()
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

macro(BEGIN_TESTCASES)
	include_sys_dir(${DUNE_STUFF_TEST_DIR}/gtest )
	add_library(gtest_dune_stuff STATIC ${DUNE_STUFF_TEST_DIR}/gtest/gtest-all.cc)

	file( GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )
	foreach( source ${test_sources} )
		get_filename_component(testname ${source} NAME_WE)
		add_executable( test_${testname} ${source} )
		add_test( test_${testname} ${CMAKE_CURRENT_BINARY_DIR}/test_${testname} )
		target_link_libraries( test_${testname} ${COMMON_LIBS} pthread gtest_dune_stuff )
		list(APPEND testnames test_${testname} )
	endforeach( source )
endmacro(BEGIN_TESTCASES)

macro(END_TESTCASES)
	add_custom_target(test_binaries DEPENDS ${testnames})
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS test_binaries)
endmacro(END_TESTCASES)

add_custom_target( config_refresh
				${CMAKE_CURRENT_SOURCE_DIR}/cmake/regen_config_header.sh ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h
				)

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

INCLUDE (CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("tr1/array" HAVE_TR1_ARRAY)
CHECK_INCLUDE_FILE_CXX("malloc.h" HAVE_MALLOC_H)

# try to use compiler flag -std=c++11
include(TestCXXAcceptsFlag)
CHECK_CXX_ACCEPTS_FLAG("-std=c++11" CXX_FLAG_CXX11)
if(CXX_FLAG_CXX11)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=c++11 ")
  set(CXX_STD0X_FLAGS "-std=c++11")
else()
  # try to use compiler flag -std=c++0x for older compilers
  CHECK_CXX_ACCEPTS_FLAG("-std=c++0x" CXX_FLAG_CXX0X)
  if(CXX_FLAG_CXX0X)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++0x" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=c++0x ")
  set(CXX_STD0X_FLAGS "-std=c++0x")
  endif(CXX_FLAG_CXX0X)
endif(CXX_FLAG_CXX11)

if(NOT CXX_STD0X_FLAGS)
    message(FATAL "you need a c++11 compatible compiler")
endif()

# __attribute__((deprecated))
CHECK_CXX_SOURCE_COMPILES("
   int main(void)
   {
     auto f = [&] (){ return 0; };
     return 0;
   };
"  HAS_LAMBDA_FUNCTIONS
)

CHECK_CXX_SOURCE_COMPILES("
		#include <vector>
		#include <iterator>
		int main(void)
		{
			std::vector<int> a;
			std::vector<int>::const_iterator b = std::begin(a);
			std::vector<int>::const_iterator e = std::end(a);
			return 0;
		};
"  HAS_STD_BEGIN_END
)


SET( CMAKE_CXX_FLAGS_RELEASE
	"-DDNDEBUG -funroll-loops -m64 -mfpmath=sse -falign-loops -mtune=native -march=native -O3 -pipe -fomit-frame-pointer " )

SET( CMAKE_CXX_FLAGS_DEBUG
	"-O0 -DDNDEBUG -g3 -ggdb -Wall -Wextra -Wc++0x-compat -Wparentheses -pedantic -Wredundant-decls -Wshadow -Wunused-variable -Winline -fno-strict-aliasing -Wundef -Wnon-virtual-dtor " )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set( CMAKE_CXX_FLAGS_DEBUG
				"${CMAKE_CXX_FLAGS_DEBUG} -Wdocumentation -Wshorten-64-to-32 -Wused-but-marked-unused -Wdisabled-macro-expansion -Wcovered-switch-default  -Wfloat-equal -Wswitch-enum -Wunreachable-code -Wnon-literal-null-conversion "
				CACHE STRING
				"Flags used by the compiler during debug builds.")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set( CMAKE_CXX_FLAGS_DEBUG
				"${CMAKE_CXX_FLAGS_DEBUG} -Wlogical-op"
				CACHE STRING
				"Flags used by the compiler during debug builds.")
endif()

FIND_PACKAGE( PkgConfig )
FIND_PACKAGE(Boost 1.48.0 REQUIRED)
ADD_DEFINITIONS( "-DHAVE_CMAKE_CONFIG ${CXX_STD0X_FLAGS}" )
INCLUDE_SYS_DIR(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

#find_package(SuperLU)
#if(SUPERLU_FOUND)
  #include_directories(${SUPERLU_INCLUDES})
  #link_directories(${SUPERLU_LIBRARY_DIRS})
#endif(SUPERLU_FOUND)

pkg_check_modules(EIGEN eigen3)
if(EIGEN_FOUND)
  INCLUDE_SYS_DIR(${EIGEN_INCLUDE_DIRS})
endif(EIGEN_FOUND)

pkg_check_modules(ALU_GRID alugrid)
pkg_check_modules(UG_GRID libug)

if(ENABLE_MPI)
	find_package(MPI REQUIRED)
	if(MPI_FOUND)
		message(STATUS "Enabling mpi features")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -w -I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi -pthread -DMPIPP_H -DENABLE_MPI=1" )
		include_directories(${MPI_CXX_INCLUDE_PATH})
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS} -pthread -L/usr/lib/openmpi/lib -lmpi -lopen-rte -lopen-pal -ldl -Wl,--export-dynamic -lnsl -lutil -lm -ldl")
		LIST( APPEND PARALIBS ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY} parmetis metis)
	else(MPI_FOUND)
		message(FATAL "mpi requested but not found")
	endif(MPI_FOUND)
endif(ENABLE_MPI)

#try to enable link-time-optimization
if (CMAKE_COMPILER_IS_GNUCC)
	if (GCC_VERSION VERSION_GREATER 4.5 OR GCC_VERSION VERSION_EQUAL 4.5)
		set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
		set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto")
		set (CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} -flto")
	endif()
endif()

ENABLE_TESTING()
