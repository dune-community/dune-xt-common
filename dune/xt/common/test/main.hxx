// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2014 - 2015)
//   Tobias Leibner  (2015)

#ifndef DUNE_AVOID_SGRID_DEPRE_WARNING_BECAUSE_I_KNOW_WHAT_IM_DOING
#define DUNE_AVOID_SGRID_DEPRE_WARNING_BECAUSE_I_KNOW_WHAT_IM_DOING 1
#endif

#include "config.h"

#ifndef THIS_IS_A_BUILDBOT_BUILD
#define THIS_IS_A_BUILDBOT_BUILD 0
#endif

#ifndef DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS
#define DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS 0
#endif
#ifndef DUNE_XT_COMMON_TEST_MAIN_ENABLE_DEBUG_LOGGING
#define DUNE_XT_COMMON_TEST_MAIN_ENABLE_DEBUG_LOGGING 0
#endif
#ifndef DUNE_XT_COMMON_TEST_MAIN_ENABLE_INFO_LOGGING
#define DUNE_XT_COMMON_TEST_MAIN_ENABLE_INFO_LOGGING 0
#endif
#ifndef DUNE_XT_COMMON_TEST_MAIN_ENABLE_TIMED_LOGGING
#define DUNE_XT_COMMON_TEST_MAIN_ENABLE_TIMED_LOGGING 0
#endif

#include <string>
#include <vector>
#include <map>
#include <random>
#include <fstream>
#include <limits>

#include <dune/common/float_cmp.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/parallel/mpihelper.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/misc/mpimanager.hh>
#endif

#include <dune/xt/common/test/gtest/gtest.h>
#include <dune/xt/common/configuration.hh>
#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/logging.hh>
#include <dune/xt/common/timedlogging.hh>
#include <dune/xt/common/convergence-study.hh>
#include <dune/xt/common/parallel/threadmanager.hh>
#include <dune/xt/common/vector.hh>

#include "common.hh"

int main(int argc, char** argv)
{
  using namespace Dune;
  using namespace Dune::XT::Common;
#if DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS
  try {
#endif

    testing::InitGoogleTest(&argc, argv);
    if (argc > 1)
      DXTC_CONFIG.read_command_line(argc, argv);
#if HAVE_DUNE_FEM
    Fem::MPIManager::initialize(argc, argv);
#else
  MPIHelper::instance(argc, argv);
#endif

    Logger().create(
#if DUNE_XT_COMMON_TEST_MAIN_ENABLE_DEBUG_LOGGING /*&& !THIS_IS_A_BUILDBOT_BUILD*/
        LOG_CONSOLE | LOG_INFO | LOG_DEBUG | LOG_ERROR
#elif DUNE_XT_COMMON_TEST_MAIN_ENABLE_INFO_LOGGING
      LOG_CONSOLE | LOG_INFO | LOG_ERROR
#else
      LOG_CONSOLE | LOG_ERROR
#endif
        ,
        "",
        "",
        "");

    TimedLogger().create(
#if DUNE_XT_COMMON_TEST_MAIN_ENABLE_TIMED_LOGGING && DUNE_XT_COMMON_TEST_MAIN_ENABLE_INFO_LOGGING
        std::numeric_limits<ssize_t>::max(),
#else
      -1,
#endif
#if DUNE_XT_COMMON_TEST_MAIN_ENABLE_TIMED_LOGGING                                                                      \
    && DUNE_XT_COMMON_TEST_MAIN_ENABLE_DEBUG_LOGGING /*&& !THIS_IS_A_BUILDBOT_BUILD*/
        std::numeric_limits<ssize_t>::max()
#else
      -1
#endif
            );
    const size_t threads = DXTC_CONFIG.has_key("threading.max_count") // <- doing this so complicated to
                               ? DXTC_CONFIG.get<size_t>("threading.max_count") //    silence the WARNING: ...
#if HAVE_TBB
                               : std::thread::hardware_concurrency();
#else
                             : 1u;
#endif
    threadManager().set_max_threads(threads);

    return RUN_ALL_TESTS();

#if DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS
  } catch (Dune::Exception& e) {
    std::cerr << "\nDune reported error: " << e.what() << std::endl;
    std::abort();
  } catch (std::exception& e) {
    std::cerr << "\n" << e.what() << std::endl;
    std::abort();
  } catch (...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
    std::abort();
  } // try
#endif // DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS
} // ... main(...)
