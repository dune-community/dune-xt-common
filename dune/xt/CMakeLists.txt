# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2013 - 2016)
#   Rene Milk       (2012 - 2016)

set(lib_dune_xt_common_sources common/filesystem.cc
  common/color.cc
  common/convergence-study.cc
  common/localization-study.cc
  common/logging.cc
  common/timedlogging.cc
  common/logstreams.cc
  common/timings.cc
  common/configuration.cc
  common/signals.cc
  common/math.cc
  common/misc.cc
  common/parallel/threadmanager.cc
  common/parallel/helper.cc
  common/string.cc
  common/test/common.cxx)

dune_library_add_sources(dunextcommon SOURCES ${lib_dune_xt_common_sources})

add_analyze(${lib_dune_xt_common_sources})

install(DIRECTORY ${DUNE_XT_COMMON_TEST_DIR}/gtest
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/
        FILES_MATCHING PATTERN "*")
install(FILES ${DUNE_XT_COMMON_TEST_DIR}/main.hxx
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/)

include_directories(SYSTEM ${DUNE_XT_COMMON_TEST_DIR}/gtest )
add_subdirectory(common/test EXCLUDE_FROM_ALL)

dune_library_add_sources(gtest_dune_xt_common
                 SOURCES common/test/gtest/gtest-all.cc)

