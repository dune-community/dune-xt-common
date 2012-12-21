PROJECT(dune-stuff)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include(DuneUtils)
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

SET( GRIDTYPE
	"YASPGRID" CACHE STRING
	"GRIDTYPE" )

SET( GRIDDIM
	"2" CACHE STRING
	"GRIDDIM" )

SET( ALUGRID_BASE_PATH
	"/opt/dune/modules/alugrid/alugrid-1.52" CACHE STRING
	"ALUGRID_BASE_PATH" )
SET( ALBERTA_GRID_BASE_PATH
	"/opt/dune/modules/alberta/alberta-3.0" CACHE STRING
	"ALUGRID_BASE_PATH" )
	
SET( ENABLE_ALUGRID OFF )
IF( ${GRIDTYPE} MATCHES "ALUGRID" )
	INCLUDE_SYS_DIR(
						${ALUGRID_BASE_PATH}/include
						${ALUGRID_BASE_PATH}/include/serial
						${ALUGRID_BASE_PATH}/include/duneinterface )
	LINK_DIRECTORIES( "${ALUGRID_BASE_PATH}/lib"  )
	SET( GRIDLIBS "alugrid" )
	SET( ENABLE_ALUGRID ON )
	IF ( ENABLE_MPI )
		INCLUDE_SYS_DIR( ${ALUGRID_BASE_PATH}/include/parallel )
	ENDIF (ENABLE_MPI )
	ADD_DEFINITIONS( "-DENABLE_ALUGRID" )
ENDIF( ${GRIDTYPE} MATCHES "ALUGRID" )

SET( ENABLE_ALBERTA OFF )
IF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )
	INCLUDE_SYS_DIR( 	${ALBERTA_GRID_BASE_PATH}/include
						${ALBERTA_GRID_BASE_PATH}/include/alberta)
	LINK_DIRECTORIES( "${ALBERTA_GRID_BASE_PATH}/lib"  )
	SET( GRIDLIBS alberta_utilities alberta_2d dunealbertagrid_2d dunegrid)
	SET( ENABLE_ALBERTA ON )
	ADD_DEFINITIONS( -DENABLE_ALBERTA=1 -DDEBUG )
ENDIF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )
	
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )
ADD_DEFINITIONS( ${CUSTOM_FLAGS} )

ADD_DUNE_MODULES( common istl grid fem geometry localfunctions)

LINK_DIRECTORIES( "/usr/lib" ${CCGNU_LIBRARY_DIRS} )

FILE( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )

set( COMMON_LIBS ${DUNE_LIBS} ${BLAS_LIB} "GL" ${PARALIBS} ${CCGNU_LIBRARIES}
									"boost_system" "boost_filesystem" "boost_timer" "boost_thread"
									${GRIDLIBS})
set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )
set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

LINK_DIRECTORIES( "/usr/lib" )


ADD_DEFINITIONS( ${MY_CXX_FLAGS} )

add_subdirectory(dune)
add_subdirectory(examples)


