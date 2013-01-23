PROJECT(dune-stuff)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

SET(ENV{PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/bin:$ENV{PATH}" )
SET(ENV{LD_LIBRARY_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib:$ENV{LD_LIBRARY_PATH}" )
SET(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )

include(DuneUtils)
include(GridUtils)
include(UseDoxygen)

#needs to come first to include correct config.h
INCLUDE_DIR( ${dune-stuff_BINARY_DIR} ${dune-stuff_SOURCE_DIR} )

SET (CMAKE_VERBOSE_MAKEFILE "on" )
#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
SET( CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".lib" ".la" ".a")
SET( CMAKE_COLOR_MAKEFILE ON)

EXECUTE_PROCESS(
	COMMAND ${CMAKE_SOURCE_DIR}/getRevision.sh
	OUTPUT_VARIABLE COMMIT
	ERROR_VARIABLE shell_error
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, setting default build type 'Debug'")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise dune-stokes
#----------------------------------------------------------------------------------------------------
set(ENABLE_MPI "0" CACHE STRING "set to 1 to enable mpi")

SET( BLAS_LIB
	"blas" CACHE STRING
	"" )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )
ADD_DEFINITIONS( ${CUSTOM_FLAGS} )

ADD_DUNE_MODULES( istl grid common fem geometry localfunctions)

LINK_DIRECTORIES( "/usr/lib" ${CCGNU_LIBRARY_DIRS} )

FILE( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )

set( COMMON_LIBS ${DUNE_LIBS} ${BLAS_LIB} "GL" ${PARALIBS} ${CCGNU_LIBRARIES}
									"boost_system" "boost_filesystem" "boost_timer" "boost_thread"
									${GRIDLIBS})
set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )
set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

add_subdirectory(dune)
add_subdirectory(examples)

