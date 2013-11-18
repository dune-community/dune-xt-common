project("dune-stuff" CXX)

# cmake specific
cmake_minimum_required( VERSION 2.8 )

set( CMAKE_COLOR_MAKEFILE ON)

#find dune-common and set the module path
find_package(dune-common)
list(APPEND CMAKE_MODULE_PATH ${dune-common_MODULE_PATH}
     "${CMAKE_SOURCE_DIR}/cmake/modules")# make sure our own modules are found

#include the dune macros
include(DuneMacros)

# start a dune project with information from dune.module
dune_project()

# dune-stuff cmake includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
execute_process(
	COMMAND ${CMAKE_SOURCE_DIR}/getRevision.sh
	OUTPUT_VARIABLE COMMIT
	ERROR_VARIABLE shell_error
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

include(DuneUtils)
# configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h )


link_directories( "/usr/lib" ${CCGNU_LIBRARY_DIRS} )

# header
file( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/dune/*.hh" )
set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )
set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

add_subdirectory(dune)
#add_subdirectory(examples)
add_subdirectory("cmake/modules")
add_subdirectory("cmake/scripts")

finalize_dune_project(GENERATE_CONFIG_H_CMAKE)
