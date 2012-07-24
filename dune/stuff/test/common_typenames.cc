#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <iostream>

#include <dune/common/mpihelper.hh>
#include <dune/stuff/common/type_utils.hh>

STUFF_TYPENAME(Dune::MPIHelper)

int main(int argc, char** argv)
{
  using namespace Dune::Stuff::Common;
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);
    std::cout << "builtin typename:\nunsigned long  -> " << Typename<unsigned long>::value()
              << "\nunknown: Dune::Exception  -> " << Typename<Dune::Exception>::value()
              << "\nextended: Dune::MPIHelper -> " << Typename<Dune::MPIHelper>::value() << std::endl;

  } catch (Dune::Exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}
