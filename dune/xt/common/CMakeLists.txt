# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017 - 2018)
#   Tobias Leibner  (2017)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# ~~~

set(lib_dune_xt_common_sources
    color.cc
    configuration.cc
    convergence-study.cc
    exceptions.cc
    filesystem.cc
    lapacke.cc
    localization-study.cc
    logging.cc
    logstreams.cc
    lpsolve.cc
    math.cc
    memory.cc
    misc.cc
    parallel/helper.cc
    parallel/threadmanager.cc
    parameter.cc
    python.cc
    signals.cc
    string.cc
    test/common.cxx
    timedlogging.cc
    timings.cc)

dune_library_add_sources(dunextcommon SOURCES ${lib_dune_xt_common_sources})

add_analyze(${lib_dune_xt_common_sources})

include_directories(SYSTEM ${DUNE_XT_COMMON_TEST_DIR}/gtest)
dune_library_add_sources(gtest_dune_xt_common SOURCES test/gtest/gtest-all.cxx)

install(DIRECTORY ${DUNE_XT_COMMON_TEST_DIR}/gtest DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/
        FILES_MATCHING
        PATTERN "*")
install(FILES ${DUNE_XT_COMMON_TEST_DIR}/main.hxx DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/xt/test/)

add_subdirectory(test EXCLUDE_FROM_ALL)
