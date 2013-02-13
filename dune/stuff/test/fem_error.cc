#include "test_common.hh"

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);
  return RUN_ALL_TESTS();
}
