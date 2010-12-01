PROJECT(dune-stuff)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include(DuneUtils)
include(UseDoxygen)
ENABLE_TESTING()

FIND_PACKAGE(Boost 1.35.0 REQUIRED)

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
	"ALUGRID_SIMPLEX" CACHE STRING
	"GRIDTYPE" )

SET( GRIDDIM
	"2" CACHE STRING
	"GRIDDIM" )

SET( POLORDER
	"1" CACHE STRING
	"POLORDER" )

SET( ALUGRID_BASE_PATH
	"/share/dune/Modules/modules_x86_64/ALUGrid-1.22" CACHE STRING
	"ALUGRID_BASE_PATH" )

SET( UGGRID_BASE_PATH
	"/share/dune/Modules/modules_x86_64/ug" CACHE STRING
	"UGGRID_BASE_PATH" )

SET( CUSTOM_FLAGS
	"-Wall -O3 -fno-strict-aliasing -std=c++0x -g -ggdb " CACHE LIST
	"CUSTOM FLAGS")

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )
#needs to come first to include correct config.h
INCLUDE_DIR( ${dune-stuff_BINARY_DIR} ${dune-stuff_SOURCE_DIR} )

IF( ${GRIDTYPE} STREQUAL "UGGRID" )
	ADD_CXX_FLAGS( -I${UGGRID_BASE_PATH}/include -DENABLE_UG )
	LINK_DIRECTORIES( ${UGGRID_BASE_PATH}/lib/ )
	SET( UGLIBS "ugS3" "ugS2" "devS")
ENDIF( ${GRIDTYPE} STREQUAL "UGGRID" )

ADD_CXX_FLAGS( ${CUSTOM_FLAGS} )
ADD_CXX_FLAGS( 
	-I${ALUGRID_BASE_PATH}/include
	-I${ALUGRID_BASE_PATH}/include/serial 
	-I${ALUGRID_BASE_PATH}/include/duneinterface -DENABLE_ALUGRID )

INCLUDE_DIR(${Boost_INCLUDE_DIR})
ADD_DUNE_MODULES( common istl grid fem )

LINK_DIRECTORIES( "/usr/lib" "${ALUGRID_BASE_PATH}/lib" "${GRAPE_PATH}" )
ADD_CXX_FLAGS( ${CXX_FLAGS} )

FILE( GLOB_RECURSE stuff "stuff/*.hh" )

set_source_files_properties( ${stuff} PROPERTIES HEADER_FILE_ONLY 1 )

ADD_EXECUTABLE(function_test src/function_test.cc ${stokes} ${stuff} )
TARGET_LINK_LIBRARIES(function_test "common" "fem" "grid" ${BLAS_LIB} "alugrid" ${UGLIBS}  "GL" ${ParaLIBS} )

HEADERCHECK( ${stuff} )
