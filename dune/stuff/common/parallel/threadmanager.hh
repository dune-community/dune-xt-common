// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_THREADMANAGER_HH
#define DUNE_STUFF_COMMON_THREADMANAGER_HH

#include <thread>
#if HAVE_TBB
#include <tbb/task_scheduler_init.h>
#endif

namespace Dune {
namespace Stuff {

struct ThreadManager;
//! global singleton ThreadManager
ThreadManager& threadManager();

/** abstractions of threading functionality
 *  currently controls tbb and forwards to dune-fem if possible, falls back to single-thread dummy imp
 **/
struct ThreadManager
{
  //! return maximal number of threads possbile in the current run
  unsigned int max_threads();

  //! return number of current threads
  unsigned int current_threads();

  //! return thread number
  unsigned int thread();

  //! set maximal number of threads available during run
  void set_max_threads(const unsigned int count);

  ~ThreadManager() = default;

private:
  friend ThreadManager& threadManager();
  //! init tbb with given thread count, prepare Eigen for smp if possible
  ThreadManager();

  unsigned int max_threads_;
#if HAVE_TBB
  std::unique_ptr<tbb::task_scheduler_init> tbb_init_;
#endif
};

inline ThreadManager& threadManager()
{
  static ThreadManager tm;
  return tm;
}
}
}

#endif // DUNE_STUFF_COMMON_THREADMANAGER_HH
