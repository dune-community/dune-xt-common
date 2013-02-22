if( NOT CMAKE_BUILD_TYPE )
  message( STATUS "No build type selected, setting default build type 'Debug'" )
  set( CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "DEBUG/RELEASE")
endif( NOT CMAKE_BUILD_TYPE )

project( dune-stuff )

# cmake specific
cmake_minimum_required( VERSION 2.8 )
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
set( CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".lib" ".la" ".a")
set( CMAKE_COLOR_MAKEFILE ON)
set( CMAKE_VERBOSE_MAKEFILE ON )

# local environment
set( ENV{PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/bin:$ENV{PATH}" )
set( ENV{LD_LIBRARY_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib:$ENV{LD_LIBRARY_PATH}" )
set( ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )

# dune-stuff cmake includes
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include(DuneUtils)
include(GridUtils)
include(UseDoxygen)

# needs to come first to include correct config.h
include_dir( ${dune-stuff_BINARY_DIR} ${dune-stuff_SOURCE_DIR} )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
execute_process(
	COMMAND ${CMAKE_SOURCE_DIR}/getRevision.sh
	OUTPUT_VARIABLE COMMIT
	ERROR_VARIABLE shell_error
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise dune-stuff
#----------------------------------------------------------------------------------------------------
set( ENABLE_MPI
     "0" CACHE STRING
     "set to 1 to enable mpi")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )

add_dune_modules(
  istl
  grid
  common
  fem
  geometry
  localfunctions
)

link_directories( "/usr/lib" ${CCGNU_LIBRARY_DIRS} )

# header
file( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )
set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )
set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

# libs
set( COMMON_LIBS
     ${DUNE_LIBS}
     ${PARALIBS}
     ${CCGNU_LIBRARIES}
     "boost_system" "boost_filesystem" "boost_timer" "boost_thread"
     ${GRIDLIBS}
)

add_subdirectory(dune)
add_subdirectory(examples)

