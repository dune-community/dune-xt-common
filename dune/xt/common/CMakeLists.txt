# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2016 - 2017)
#   Rene Milk       (2015)

set(lib_dune_xt_common_sources
    filesystem.cc
    color.cc
    convergence-study.cc
    localization-study.cc
    logging.cc
    timedlogging.cc
    logstreams.cc
    timings.cc
    configuration.cc
    signals.cc
    math.cc
    misc.cc
    parallel/threadmanager.cc
    parallel/helper.cc
    string.cc
    test/common.cxx
   )

dune_library_add_sources(dunextcommon SOURCES ${lib_dune_xt_common_sources})

add_analyze(${lib_dune_xt_common_sources})

if(dune-pybindxi_FOUND)
  dune_pybindxi_add_module(_common bindings.cc)
  target_link_dune_default_libraries(_common)
  if(DUNE_XT_WITH_PYTHON_BINDINGS)
    add_custom_target(bindings ALL DEPENDS _common)
  else()
    add_custom_target(bindings DEPENDS _common)
  endif()
endif()

include_directories(SYSTEM ${DUNE_XT_COMMON_TEST_DIR}/gtest)
dune_library_add_sources(gtest_dune_xt_common SOURCES test/gtest/gtest-all.cc)

install(DIRECTORY ${DUNE_XT_COMMON_TEST_DIR}/gtest
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/
        FILES_MATCHING PATTERN "*")
install(FILES ${DUNE_XT_COMMON_TEST_DIR}/main.hxx
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/)

add_subdirectory(test EXCLUDE_FROM_ALL)

