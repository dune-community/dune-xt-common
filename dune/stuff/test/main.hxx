// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

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

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/float_cmp.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/parallel/mpihelper.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/misc/mpimanager.hh>
#endif
#include <dune/stuff/common/reenable_warnings.hh>

#include <dune/stuff/test/gtest/gtest.h>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/configuration.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/timedlogging.hh>
#include <dune/stuff/common/convergence-study.hh>

#include "common.hh"

#if HAVE_TBB
#include <thread>
#include <tbb/task_scheduler_init.h>
#endif

class DUNE_DEPRECATED_MSG("Use the expectation macros of the gtest test suite (20.08.2014)!") errors_are_not_as_expected
    : public Dune::Exception
{
};


std::vector<double> DUNE_DEPRECATED_MSG("Use the expectation macros of the gtest test suite (20.08.2014)!")
    truncate_vector(const std::vector<double>& in, const size_t size)
{
  assert(size <= in.size());
  if (size == in.size())
    return in;
  else {
    std::vector<double> ret(size);
    for (size_t ii = 0; ii < size; ++ii)
      ret[ii] = in[ii];
    return ret;
  }
} // ... truncate_vector(...)


int main(int argc, char** argv)
{
#if DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS
  try {
#endif

    testing::InitGoogleTest(&argc, argv);
    DSC_CONFIG.read_options(argc, argv);
#if HAVE_DUNE_FEM
    Dune::Fem::MPIManager::initialize(argc, argv);
#else
  Dune::MPIHelper::instance(argc, argv);
#endif

    DSC::Logger().create(
#if DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING
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
#if DUNE_STUFF_TEST_MAIN_ENABLE_TIMED_LOGGING && DUNE_STUFF_TEST_MAIN_ENABLE_DEBUG_LOGGING
        std::numeric_limits<ssize_t>::max()
#else
      -1
#endif
            );
#if HAVE_TBB
    tbb::task_scheduler_init tbb_init(DSC_CONFIG.has_key("threading.max_count") // <- doing this so complicated to
                                          ? DSC_CONFIG.get<int>("threading.max_count") //    silence the WARNING: ...
                                          : 1);
#endif
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
