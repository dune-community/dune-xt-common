// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2013 - 2015)
//   Tobias Leibner  (2014 - 2015)

#include "config.h"

#if HAVE_TBB
#include <thread>
#endif

#include <boost/numeric/conversion/cast.hpp>

#if HAVE_EIGEN
#include <dune/xt/common/disable_warnings.hh>
#include <Eigen/Core>
#include <dune/xt/common/reenable_warnings.hh>
#endif

#if HAVE_TBB
#include <tbb/task_scheduler_init.h>
#endif

#include <dune/common/exceptions.hh>

#include <dune/xt/common/configuration.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/misc/threads/threadmanager.hh>
#endif

#include "threadmanager.hh"

// some assertions only make sense if dune-fem's threading manager is non-trivial
#if (defined(USE_PTHREADS) || defined(_OPENMP)) && HAVE_DUNE_FEM
#define WITH_DUNE_FEM_AND_THREADING(expr) expr
#else
#define WITH_DUNE_FEM_AND_THREADING(expr)
#endif

#if HAVE_TBB

size_t Dune::XT::Common::ThreadManager::max_threads()
{
  const auto threads = DXTC_CONFIG_GET("threading.max_count", 1);
  WITH_DUNE_FEM_AND_THREADING(assert(Dune::Fem::ThreadManager::maxThreads() == threads);)
  return threads;
}

size_t Dune::XT::Common::ThreadManager::current_threads()
{
  const auto threads = max_threads();
  WITH_DUNE_FEM_AND_THREADING(assert(long(Dune::Fem::ThreadManager::currentThreads()) == long(threads));)
  return threads;
}

size_t Dune::XT::Common::ThreadManager::thread()
{
  const auto tbb_id = std::this_thread::get_id();
  static std::map<decltype(tbb_id), size_t> thread_ids;
  const auto it = thread_ids.find(tbb_id);
  if (it == thread_ids.end())
    map_emplace(thread_ids, tbb_id, thread_ids.size());
  return thread_ids.at(tbb_id);
}

//! both std::hw_concur and intel's default_thread_count fail for mic
size_t Dune::XT::Common::ThreadManager::default_max_threads()
{
#ifndef __MIC__
  return std::thread::hardware_concurrency();
#else
  return DS_MAX_MIC_THREADS;
#endif
}

void Dune::XT::Common::ThreadManager::set_max_threads(const size_t count)
{
  DXTC_CONFIG.set("threading.max_count", count, true);
  max_threads_ = count;
  WITH_DUNE_FEM_AND_THREADING(Dune::Fem::ThreadManager::setMaxNumberThreads(boost::numeric_cast<int>(count));)
#if HAVE_EIGEN
    Eigen::setNbThreads(boost::numeric_cast<int>(count));
#endif
}

Dune::XT::Common::ThreadManager::ThreadManager()
  : max_threads_(default_max_threads())
{
#if HAVE_EIGEN
  // must be called before tbb threads are created via tbb::task_scheduler_init object ctor
  Eigen::initParallel();
  Eigen::setNbThreads(1);
#endif
  WITH_DUNE_FEM_AND_THREADING(Dune::Fem::ThreadManager::setMaxNumberThreads(1);)
}

#else // if HAVE_TBB

size_t Dune::XT::Common::ThreadManager::max_threads()
{
  return 1;
}

size_t Dune::XT::Common::ThreadManager::current_threads()
{
  return 1;
}

size_t Dune::XT::Common::ThreadManager::thread()
{
  return 0;
}

void Dune::XT::Common::ThreadManager::set_max_threads(const size_t count)
{
  if (count > 1)
    DUNE_THROW(InvalidStateException, "Trying to use more than one thread w/o TBB");
}

size_t Dune::XT::Common::ThreadManager::default_max_threads()
{
  return 1;
}

Dune::XT::Common::ThreadManager::ThreadManager()
  : max_threads_(1)
{
}

#endif // HAVE_TBB
