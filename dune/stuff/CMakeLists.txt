# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

set(lib_dune_stuff_sources common/filesystem.cc 
  common/color.cc
  common/convergence-study.cc
  common/localization-study.cc
  common/logging.cc
  common/logstreams.cc
  common/profiler.cc
  common/configuration.cc
  common/signals.cc
  common/math.cc
  common/parallel/threadmanager.cc
  common/parallel/helper.cc
  fem/femeoc.cc
  grid/fakeentity.cc 
  functions/expression/mathexpr.cc
  la/container/pattern.cc
  test/common.cxx)

dune_add_library("dunestuff" ${lib_dune_stuff_sources}
  ADD_LIBS ${DUNE_LIBS})
target_link_dune_default_libraries(dunestuff)
if(dune-grid_FOUND)
  add_dune_alugrid_flags(dunestuff)
endif(dune-grid_FOUND)

add_analyze(${lib_dune_stuff_sources})

add_subdirectory(test EXCLUDE_FROM_ALL)
