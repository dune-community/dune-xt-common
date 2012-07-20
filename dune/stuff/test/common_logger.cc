#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/stuff/common/logging.hh>

namespace DSC = Dune::Stuff::Common;

int main(int, char* [])
{
  DSC::Logger().create();
  DSC::Logger().err() << "wargh" << std::endl;
  return 0;
}
