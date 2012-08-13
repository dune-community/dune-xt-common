#include "test_common.hh"

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/logstreams.hh>

namespace DSC = Dune::Stuff::Common;

void balh(std::ostream& out)
{
  static int c = 0;
  out << "balh " << c << "\n";
  c++;
}

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
  DSC_LOG_ERROR.suspend();
  DSC_LOG_ERROR << "not in output\n";
  balh(DSC_LOG_ERROR);
  DSC_LOG_ERROR.resume();
  DSC_LOG_ERROR << "in output\n";
  balh(DSC_LOG_ERROR);

  // this should do nothitng whatsoever
  balh(DSC::dev_null);
  DSC::Logger().flush();
  return 0;
}
