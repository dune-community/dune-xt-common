# This file is part of the dune-stuff project:
#   http://users.dune-project.org/projects/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

project("dune-stuff" CXX)

if(CMAKE_WITH_AUTOTOOLS)
  # this is how it used to be in the good old days :)
  # this assumes that we use autotools to configure everything and just use this CMakeLists.txt for the tests and
  # examples in dune-gdt
  if( NOT CMAKE_BUILD_TYPE )
    message( STATUS "No build type selected, setting default build type 'Debug'" )
    set( CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "DEBUG/RELEASE")
  endif( NOT CMAKE_BUILD_TYPE )

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

  # needs to come first to include correct config.h
  include_directories( ${dune-stuff_SOURCE_DIR} )
  include_directories( ${dune-stuff_BINARY_DIR} )

  # dune-stuff cmake includes
  set(CMAKE_MODULE_PATH
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
  )
  include(DuneUtils)
  include(GridUtils)

  # we would like to have fasp
  #include(FindFASP)

  add_dune_modules(
    stuff
    common
    pdelab
    typetree
    fem
    istl
    geometry
    grid
    localfunctions
  )

  # header
  file( GLOB_RECURSE stuff "${CMAKE_CURRENT_SOURCE_DIR}/*.hh" )
  set( COMMON_HEADER ${stuff} ${DUNE_HEADERS} )
  set_source_files_properties( ${COMMON_HEADER} PROPERTIES HEADER_FILE_ONLY 1 )

  # libs
  set( COMMON_LIBS
      ${DUNE_LIBS}
      ${DUNE_DEFAULT_LIBS}
      ${PARALIBS}
      ${CCGNU_LIBRARIES}
      ${Boost_SYSTEM_LIBS}
      ${GRIDLIBS}
      ${FASPLIB}
  )

  add_subdirectory(dune/stuff/test)

  ADD_EXECUTABLE(this_needs_to_be_here_to_have_header_listing_until_we_have_a_real_executable
                 "dune/stuff/test/common_color.cc"
                 ${COMMON_HEADER} )
  TARGET_LINK_LIBRARIES(this_needs_to_be_here_to_have_header_listing_until_we_have_a_real_executable
                        ${COMMON_LIBS})

else(CMAKE_WITH_AUTOTOOLS)
  # this assumes that we run dunecontrol with the --use-cmake option

  # cmake specific
  cmake_minimum_required( VERSION 2.8 )

  set( CMAKE_COLOR_MAKEFILE ON)

  # guess dune-common build dir
  if(NOT (dune-common_DIR OR dune-common_ROOT OR
          "${CMAKE_PREFIX_PATH}" MATCHES ".*dune-common.*"))
      string(REPLACE  ${CMAKE_PROJECT_NAME} dune-common dune-common_DIR
        ${PROJECT_BINARY_DIR})
  endif()

  #find dune-common and set the module path
  find_package(dune-common REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${dune-common_MODULE_PATH}
    "${PROJECT_SOURCE_DIR}/cmake/modules")

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
endif(CMAKE_WITH_AUTOTOOLS)
