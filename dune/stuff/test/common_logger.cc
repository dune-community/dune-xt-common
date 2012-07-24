#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/stuff/common/logging.hh>

namespace DSC = Dune::Stuff::Common;

int main(int, char* [])
{
  DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);
  DSC::Logger().error() << "This should be in output\n";
  DSC::Logger().info() << "This should NOT be in output\n";
  DSC::Logger().debug() << "dito\n";
  DSC::Logger().flush();
  for (int i : {DSC::LOG_INFO, DSC::LOG_DEBUG, DSC::LOG_ERROR}) {
    const int id = DSC::Logger().addStream(DSC::LOG_CONSOLE | i);
    DSC::Logger().getStream(id) << "Create a new stream with id: " << id << std::endl;
  }
  DSC::Logger().flush();
  return 0;
}
