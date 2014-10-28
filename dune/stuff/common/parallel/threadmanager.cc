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

#if HAVE_TBB

#include <tbb/compat/thread>

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  const auto threads = DSC_CONFIG_GET("threading.max_count", 1);
  WITH_DUNE_FEM(assert(Dune::Fem::ThreadManager::maxThreads() == threads);)
  return threads;
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  const auto threads = max_threads();
  WITH_DUNE_FEM(assert(long(Dune::Fem::ThreadManager::currentThreads()) == long(threads));)
  return threads;
}

unsigned int Dune::Stuff::ThreadManager::thread()
{
  const auto tbb_id = tbb::this_tbb_thread::get_id();
  static std::map<decltype(tbb_id), unsigned int> thread_ids;
  const auto it = thread_ids.find(tbb_id);
  if (it == thread_ids.end())
    thread_ids.emplace(tbb_id, thread_ids.size());
  return thread_ids.at(tbb_id);
}

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int count)
{
  DSC_CONFIG.set("threading.max_count", count, true /*overwrite*/);
  WITH_DUNE_FEM(Dune::Fem::ThreadManager::setMaxNumberThreads(count);)
}

#else // if HAVE_TBB

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  return 1;
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  return 1;
}

unsigned int Dune::Stuff::ThreadManager::thread()
{
  return 1;
}

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int count)
{
  if (count > 1)
    DUNE_THROW(InvalidStateException, "Trying to use more than one thread w/o TBB");
}

#endif // HAVE_DUNE_FEM
