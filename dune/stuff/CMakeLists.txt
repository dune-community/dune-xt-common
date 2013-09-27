set(lib_dune_stuff_sources common/filesystem.cc 
  common/color.cc 
  common/logging.cc 
  common/logstreams.cc
  common/profiler.cc
  common/parameter/configcontainer.cc
  common/parameter/tree.cc
  common/signals.cc
  common/math.cc
  fem/femeoc.cc
  functions/expression/mathexpr.cc )

dune_add_library("dunestuff" ${lib_dune_stuff_sources}
  ADD_LIBS ${DUNE_LIBS})
target_link_dune_default_libraries(dunestuff)

add_analyze(${lib_dune_stuff_sources})

add_subdirectory(test EXCLUDE_FROM_ALL)
