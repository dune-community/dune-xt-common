// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_THREADMANAGER_HH
#define DUNE_STUFF_COMMON_THREADMANAGER_HH

#include <deque>
#include <algorithm>
#include <type_traits>
#if HAVE_TBB
#include <tbb/enumerable_thread_specific.h>
#endif
#include <boost/noncopyable.hpp>

#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/memory.hh>

namespace Dune {
namespace Stuff {

/** abstractions of threading functionality
 *  currently forwads to dune-fem if possible, falls back to single-thread dummy imp
 **/
struct ThreadManager
{
  //! return maximal number of threads possbile in the current run
  static unsigned int max_threads();

  //! return number of current threads
  static unsigned int current_threads();

  //! return thread number
  static unsigned int thread();

  //! set maximal number of threads available during run
  static void set_max_threads(const unsigned int count);

private:
  //  std::unique_ptr<tbb::task_scheduler_init> tbb_init_;
};

friend ThreadManager& threadManager();
//! init tbb with given thread count, prepare Eigen for smp if possible
ThreadManager(unsigned int max_threads = std::thread::hardware_concurrency());

unsigned int max_threads_;
#if HAVE_TBB
tbb::task_scheduler_init tbb_init_;
#endif
};
}
}

#endif // DUNE_STUFF_COMMON_THREADMANAGER_HH
