#include "test_common.hh"

#include <dune/stuff/common/profiler.hh>

int main(int argc, char** argv)
{
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);
    DSC_PROF.startTiming("koko");
    DSC_PROF.stopTiming("koko");

  } catch (Dune::Exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}
