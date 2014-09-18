// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "threadmanager.hh"

#include <dune/stuff/common/configuration.hh>

#if HAVE_DUNE_FEM

#include <dune/fem/misc/threads/threadmanager.hh>

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  return Dune::Fem::ThreadManager::maxThreads();
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  return Dune::Fem::ThreadManager::currentThreads();
}

unsigned int Dune::Stuff::ThreadManager::thread()
{
  return Dune::Fem::ThreadManager::thread();
}

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int count)
{
  Dune::Fem::ThreadManager::setMaxNumberThreads(count);
}

#elif HAVE_TBB

#include <tbb/compat/thread>

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  return DSC_CONFIG_GET("threading.max_count", 1);
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  return max_threads();
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
}


#else // if HAVE_DUNE_FEM

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

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int /*count*/)
{
}

#endif // HAVE_DUNE_FEM
