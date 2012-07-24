#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/common/mpihelper.hh>
#include <dune/stuff/common/math.hh>
#include <dune/common/float_cmp.hh>

#define MY_ASSERT(cond)                                                                                                \
  if (not cond) {                                                                                                      \
    DUNE_THROW(Dune::Exception, #cond " failed");                                                                      \
  }

template <class T>
struct Test
{
  static void run()
  {
    using namespace Dune::Stuff::Common::Math;
    MinMaxAvg<T> mma;
    mma(-1);
    mma(1);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(0.0)));
    mma(0);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(0.0)));
    mma(-4);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-4.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(-1.0)));
  }
};

int main(int argc, char** argv)
{
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);
    Test<double>::run();
    Test<int>::run();

  } catch (Dune::Exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}
