# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

set(lib_dune_stuff_sources common/filesystem.cc 
  common/color.cc
  common/convergence-study.cc
  common/localization-study.cc
  common/logging.cc
  common/timedlogging.cc
  common/logstreams.cc
  common/profiler.cc
  common/configuration.cc
  common/signals.cc
  common/math.cc
  common/misc.cc
  common/parallel/threadmanager.cc
  common/parallel/helper.cc
  grid/fakeentity.cc 
  functions/expression/mathexpr.cc
  la/container/pattern.cc
  test/common.cxx)

dune_add_library("dunestuff" ${lib_dune_stuff_sources}
  ADD_LIBS ${DUNE_LIBS})
add_dune_mpi_flags(dunestuff)
if(TBB_FOUND)
  add_dune_tbb_flags(dunestuff)
endif(TBB_FOUND)

if(dune-grid_FOUND)
  add_dune_superlu_flags(dunestuff)
  add_dune_alugrid_flags(dunestuff)
endif(dune-grid_FOUND)
target_link_dune_default_libraries(dunestuff)

add_analyze(${lib_dune_stuff_sources})
FILE( GLOB_RECURSE _header "${CMAKE_CURRENT_SOURCE_DIR}/*.hh" )
add_format(${lib_dune_stuff_sources} ${_header})

add_subdirectory(test EXCLUDE_FROM_ALL)
