// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "threadmanager.hh"

#include <dune/stuff/common/configuration.hh>
#include <dune/common/exceptions.hh>

#include <dune/stuff/fem/namespace.hh>
#if HAVE_DUNE_FEM
#include <dune/fem/misc/threads/threadmanager.hh>
#endif

#if HAVE_EIGEN
#include <Eigen/Core>
#endif

#if HAVE_TBB

#include <tbb/compat/thread>

size_t Dune::Stuff::ThreadManager::max_threads()
{
  const auto threads = DSC_CONFIG_GET("threading.max_count", 1);
  WITH_DUNE_FEM(assert(Dune::Fem::ThreadManager::maxThreads() == threads);)
  return threads;
}

size_t Dune::Stuff::ThreadManager::current_threads()
{
  const auto threads = max_threads();
  WITH_DUNE_FEM(assert(long(Dune::Fem::ThreadManager::currentThreads()) == long(threads));)
  return threads;
}

size_t Dune::Stuff::ThreadManager::thread()
{
  const auto tbb_id = tbb::this_tbb_thread::get_id();
  static std::map<decltype(tbb_id), size_t> thread_ids;
  const auto it = thread_ids.find(tbb_id);
  if (it == thread_ids.end())
    thread_ids.emplace(tbb_id, thread_ids.size());
  return thread_ids.at(tbb_id);
}

void Dune::Stuff::ThreadManager::set_max_threads(const size_t count)
{
  max_threads_ = count;
  WITH_DUNE_FEM(Dune::Fem::ThreadManager::setMaxNumberThreads(count);)
#if HAVE_EIGEN
  Eigen::setNbThreads(count);
#endif
  tbb_init_->terminate();
  tbb_init_->initialize(count);
}

Dune::Stuff::ThreadManager::ThreadManager()
  : max_threads_(1)
  , tbb_init_(nullptr)
{
#if HAVE_EIGEN
  // must be called before tbb threads are created via tbb::task_scheduler_init object ctor
  Eigen::initParallel();
  Eigen::setNbThreads(1);
#endif
  tbb_init_ = Common::make_unique<tbb::task_scheduler_init>(max_threads_);
  set_max_threads(std::thread::hardware_concurrency());
}

#else // if HAVE_TBB

size_t Dune::Stuff::ThreadManager::max_threads()
{
  return 1;
}

size_t Dune::Stuff::ThreadManager::current_threads()
{
  return 1;
}

size_t Dune::Stuff::ThreadManager::thread()
{
  return 1;
}

void Dune::Stuff::ThreadManager::set_max_threads(const size_t count)
{
  if (count > 1)
    DUNE_THROW(InvalidStateException, "Trying to use more than one thread w/o TBB");
}

Dune::Stuff::ThreadManager::ThreadManager()
  : max_threads_(1)
{
}

#endif // HAVE_DUNE_FEM
