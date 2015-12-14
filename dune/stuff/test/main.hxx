// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2015)
//   Rene Milk       (2014 - 2015)

#include "config.h"

#ifndef THIS_IS_A_BUILDBOT_BUILD
#define THIS_IS_A_BUILDBOT_BUILD 0
#endif

#ifndef DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS
#define DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS 0
#endif
#ifndef DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING
#define DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING 0
#endif
#ifndef DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING
#define DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING 0
#endif
#ifndef DUNE_STUFF_TEST_MAIN_ENABLE_TIMED_LOGGING
#define DUNE_STUFF_TEST_MAIN_ENABLE_TIMED_LOGGING 0
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

#include <dune/stuff/test/gtest/gtest.h>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/configuration.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/timedlogging.hh>
#include <dune/stuff/common/convergence-study.hh>
#include <dune/stuff/common/parallel/threadmanager.hh>
#include <dune/stuff/common/vector.hh>

#include "common.hh"

int main(int argc, char** argv)
{
#if DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS
  try {
#endif

    testing::InitGoogleTest(&argc, argv);
    if (argc > 1)
      DSC_CONFIG.read_command_line(argc, argv);
#if HAVE_DUNE_FEM
    Dune::Fem::MPIManager::initialize(argc, argv);
#else
  Dune::MPIHelper::instance(argc, argv);
#endif

    DSC::Logger().create(
#if DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING /*&& !THIS_IS_A_BUILDBOT_BUILD*/
        DSC::LOG_CONSOLE | DSC::LOG_INFO | DSC::LOG_DEBUG | DSC::LOG_ERROR
#elif DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING
      DSC::LOG_CONSOLE | DSC::LOG_INFO | DSC::LOG_ERROR
#else
      DSC::LOG_CONSOLE | DSC::LOG_ERROR
#endif
        ,
        "",
        "",
        "");

    DSC::TimedLogger().create(
#if DUNE_STUFF_TEST_MAIN_ENABLE_TIMED_LOGGING && DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING
        std::numeric_limits<ssize_t>::max(),
#else
      -1,
#endif
#if DUNE_STUFF_TEST_MAIN_ENABLE_TIMED_LOGGING                                                                          \
    && DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING /*&& !THIS_IS_A_BUILDBOT_BUILD*/
        std::numeric_limits<ssize_t>::max()
#else
      -1
#endif
            );
    const size_t threads = DSC_CONFIG.has_key("threading.max_count") // <- doing this so complicated to
                               ? DSC_CONFIG.get<size_t>("threading.max_count") //    silence the WARNING: ...
#if HAVE_TBB
                               : std::thread::hardware_concurrency();
#else
                             : 1u;
#endif
    DS::threadManager().set_max_threads(threads);

    return RUN_ALL_TESTS();

#if DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS
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
#endif // DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS
} // ... main(...)
