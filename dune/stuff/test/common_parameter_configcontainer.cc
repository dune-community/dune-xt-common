#include "test_common.hh"

#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>

int main(int argc, char** argv)
{
  using namespace Dune::Stuff::Common::Parameter;
  testing::InitGoogleTest(&argc, argv);
  DSC_CONFIG_GET("koko", 9);
  DSC_CONFIG_GETV("koko", 8, ValidateGreater<int>(7));
  DSC_CONFIG_GET("koko", 9);
  DSC_CONFIG_GET("koko", 91);
  DSC_CONFIG.printRequests(std::cout);
  DSC_CONFIG.printMismatchedDefaults(std::cout);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
