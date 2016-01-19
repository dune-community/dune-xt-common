// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2013 - 2015)

#ifndef DUNE_XT_COMMON_THREADMANAGER_HH
#define DUNE_XT_COMMON_THREADMANAGER_HH

#include <thread>
#if HAVE_TBB
#include <tbb/task_scheduler_init.h>
#endif

namespace Dune {
namespace XT {
namespace Common {

struct ThreadManager;
//! global singleton ThreadManager
ThreadManager& threadManager();

/** abstractions of threading functionality
 *  currently controls tbb and forwards to dune-fem if possible, falls back to single-thread dummy imp
 **/
struct ThreadManager
{
  static size_t default_max_threads();

  //! return maximal number of threads possbile in the current run
  size_t max_threads();

  //! return number of current threads
  size_t current_threads();

  //! return thread number
  size_t thread();

  //! set maximal number of threads available during run
  void set_max_threads(const size_t count);

  ~ThreadManager() = default;

private:
  friend ThreadManager& threadManager();
  //! init tbb with given thread count, prepare Eigen for smp if possible
  ThreadManager();

  size_t max_threads_;
#if HAVE_TBB
  tbb::task_scheduler_init tbb_init_;
#endif
};

inline ThreadManager& threadManager()
{
  static ThreadManager tm;
  return tm;
}

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_THREADMANAGER_HH
