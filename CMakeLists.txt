PROJECT(dune-stuff)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include(DuneUtils)
include(UseDoxygen)

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

#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise dune-stokes
#----------------------------------------------------------------------------------------------------
SET( BLAS_LIB
	"blas" CACHE STRING
	"" )

SET( GRIDTYPE
	"YASPGRID" CACHE STRING
	"GRIDTYPE" )

SET( GRIDDIM
	"2" CACHE STRING
	"GRIDDIM" )


CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )
#needs to come first to include correct config.h
INCLUDE_DIR( ${dune-stuff_BINARY_DIR} ${dune-stuff_SOURCE_DIR} )
ADD_CXX_FLAGS( ${CUSTOM_FLAGS} )

ADD_DUNE_MODULES( common istl grid fem geometry localfunctions stuff)

LINK_DIRECTORIES( "/usr/lib" ${CCGNU_LIBRARY_DIRS} )

FILE( GLOB header "src/*.hh" )
FILE( GLOB_RECURSE stuff "stuff/*.hh" )

set( COMMON_LIBS "grid" "fem" "common"  ${BLAS_LIB} "GL" ${ParaLIBS} ${CCGNU_LIBRARIES} "boost_filesystem")
set( COMMON_HEADER ${header} ${stuff} ${DUNE_HEADERS} )
set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

LINK_DIRECTORIES( "/usr/lib" )

FILE( GLOB_RECURSE stuff "stuff/*.hh" )

HEADERCHECK( ${stuff} )
ADD_DEFINITIONS( ${MY_CXX_FLAGS} )

add_subdirectory(dune)