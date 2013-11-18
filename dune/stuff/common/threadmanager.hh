#ifndef DUNE_STUFF_COMMON_THREADMANAGER_HH
#define DUNE_STUFF_COMMON_THREADMANAGER_HH

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
};
}
}

#endif // DUNE_STUFF_COMMON_THREADMANAGER_HH
