dune_add_library("dunestuff"  common/filesystem.cc 
  common/color.cc 
  common/logging.cc 
  common/logstreams.cc
  common/profiler.cc
  common/parameter/configcontainer.cc
  common/parameter/tree.cc
  common/signals.cc
  common/math.cc
  fem/femeoc.cc
  function/expression/mathexpr.cc
  ADD_LIBS ${DUNE_LIBS})
target_link_dune_default_libraries(dunestuff)

add_subdirectory(test EXCLUDE_FROM_ALL)
