#include <config.h>

#include "memory.hh"

#include <dune/xt/common/timings.hh>
#include <dune/xt/common/filesystem.hh>
#include <dune/xt/common/configuration.hh>

#include <sys/resource.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Dune {
namespace XT {
namespace Common {

void mem_usage(std::string filename)
{
  auto comm = Dune::MPIHelper::getCollectiveCommunication();
  // Compute the peak memory consumption of each processes
  int who = RUSAGE_SELF;
  struct rusage usage;
  getrusage(who, &usage);
  long peakMemConsumption = usage.ru_maxrss;
  // compute the maximum and mean peak memory consumption over all processes
  long maxPeakMemConsumption = comm.max(peakMemConsumption);
  long totalPeakMemConsumption = comm.sum(peakMemConsumption);
  long meanPeakMemConsumption = totalPeakMemConsumption / comm.size();
  // write output on rank zero
  if (comm.rank() == 0) {
    std::unique_ptr<boost::filesystem::ofstream> memoryConsFile(make_ofstream(filename));
    *memoryConsFile << "global.maxPeakMemoryConsumption,global.meanPeakMemoryConsumption\n"
                    << maxPeakMemConsumption << "," << meanPeakMemConsumption << std::endl;
  }
}

void mem_usage()
{
  mem_usage(std::string(DXTC_CONFIG_GET("global.datadir", "data/")) + std::string("/memory.csv"));
}

} // namespace Common
} // namespace XT
} // namespace Dune
