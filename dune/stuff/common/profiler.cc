#include <dune/common/version.hh>

#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include "profiler.hh"

#if HAVE_LIKWID
#include <likwid.h>
#define DSC_LIKWID_BEGIN_SECTION(name)                                                                                 \
  LIKWID_MARKER_INIT;                                                                                                  \
  LIKWID_MARKER_START(name);
#define DSC_LIKWID_END_SECTION(name)                                                                                   \
  LIKWID_MARKER_STOP(name);                                                                                            \
  LIKWID_MARKER_CLOSE;
#else
#define DSC_LIKWID_BEGIN_SECTION(name)
#define DSC_LIKWID_END_SECTION(name)
#endif

#include <dune/stuff/common/disable_warnings.hh>
#if DUNE_VERSION_NEWER(DUNE_COMMON, 2, 3)
#include <dune/common/parallel/mpihelper.hh>
#else
#include <dune/common/mpihelper.hh>
#endif
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/filesystem.hh>

#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/config.hpp>
#include <boost/timer/timer.hpp>


namespace Dune {
namespace Stuff {
namespace Common {

TimingData::TimingData(const std::string _name)
  : timer_(new boost::timer::cpu_timer)
  , name(_name)
{
  timer_->start();
}

void TimingData::stop()
{
  timer_->stop();
}

boost::timer::nanosecond_type TimingData::delta() const
{
  return (timer_->elapsed().user + timer_->elapsed().system) / boost::timer::nanosecond_type(1e6);
}

void Profiler::startTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + toString(i);
  DSC_LIKWID_BEGIN_SECTION(section)
  startTiming(section);
}

long Profiler::stopTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + toString(i);
  DSC_LIKWID_END_SECTION(section)
  return stopTiming(section);
}

long Profiler::getTiming(const std::string section_name, const int i) const
{
  const std::string section = section_name + toString(i);
  return getTiming(section);
}

void Profiler::resetTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + toString(i);
  return resetTiming(section);
}

void Profiler::resetTiming(const std::string section_name)
{
  try {
    stopTiming(section_name);
  } catch (Dune::RangeError) {
    // ok, timer simply wasn't running
  }
  Datamap& current_data      = datamaps_[current_run_number_];
  current_data[section_name] = 0;
}

void Profiler::startTiming(const std::string section_name)
{
  if (current_run_number_ >= datamaps_.size()) {
    datamaps_.push_back(Datamap());
  }

  const KnownTimersMap::iterator section = known_timers_map_.find(section_name);
  if (section != known_timers_map_.end()) {
    if (section->second.first) // timer currently running
      return;

    section->second.first  = true; // set active, start with new
    section->second.second = TimingData(section_name);
  } else {
    // init new section
    known_timers_map_[section_name] = std::make_pair(true, TimingData(section_name));
  }
} // StartTiming

long Profiler::stopTiming(const std::string section_name)
{
  assert(current_run_number_ < datamaps_.size());
  if (known_timers_map_.find(section_name) == known_timers_map_.end())
    DUNE_THROW(Dune::RangeError, "trying to stop timer " << section_name << " that wasn't started\n");

  known_timers_map_[section_name].first = false; // marks as not running
  TimingData& timing = known_timers_map_[section_name].second;
  timing.stop();
  long delta            = timing.delta();
  Datamap& current_data = datamaps_[current_run_number_];
  if (current_data.find(section_name) == current_data.end())
    current_data[section_name] = delta;
  else
    current_data[section_name] += delta;
  return delta;
} // StopTiming

long Profiler::getTiming(const std::string section_name) const
{
  assert(current_run_number_ < datamaps_.size());
  return getTimingIdx(section_name, current_run_number_);
}

long Profiler::getTimingIdx(const std::string section_name, const int run_number) const
{
  assert(run_number < int(datamaps_.size()));
  const Datamap& data             = datamaps_[run_number];
  Datamap::const_iterator section = data.find(section_name);
  if (section == data.end()) {
    // timer might still be running
    const auto& timer_it = known_timers_map_.find(section_name);
    if (timer_it == known_timers_map_.end())
      DUNE_THROW(Dune::InvalidStateException, "no timer found: " + section_name);
    return timer_it->second.second.delta();
  }
  return section->second;
} // GetTiming


void Profiler::reset(const int numRuns)
{
  if (!(numRuns > 0))
    DUNE_THROW(Dune::RangeError, "preparing the profiler for 0 runs is moronic");
  datamaps_.clear();
  datamaps_           = DatamapVector(numRuns, Datamap());
  current_run_number_ = 0;
} // Reset

void Profiler::addCount(const int num)
{
  counters_[num] += 1;
}

void Profiler::nextRun()
{
  // set all known timers to "stopped"
  for (auto& timer_it : known_timers_map_)
    timer_it.second.first = false;
  current_run_number_++;
}

void Profiler::outputAveraged(const int refineLevel, const long numDofs, const double scale_factor) const
{
  const auto& comm   = Dune::MPIHelper::getCollectiveCommunication();
  const int numProce = comm.size();

  boost::format csv_name("p%d_refinelvl_%d.csv");
  csv_name % numProce % refineLevel;
  boost::filesystem::path filename(output_dir_);
  filename /= csv_name.str();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << filename << std::endl;

#ifndef NDEBUG
  for (const auto& count : counters_) {
    std::cout << "proc " << comm.rank() << " bId " << count.first << " count " << count.second << std::endl;
  }
#endif // ifndef NDEBUG

  boost::filesystem::ofstream csv(filename);

  std::map<std::string, long> averages_map;
  for (const auto& datamap : datamaps_) {
    for (const auto& timing : datamap) {
      //! this used to be GetTiming( it->second ), which is only valid thru an implicit and wrong conversion..
      averages_map[timing.first] += getTiming(timing.first);
    }
  }

  // outputs column names
  csv << "refine" << csv_sep << "processes" << csv_sep << "numDofs" << csv_sep << "L1 error" << csv_sep;
  for (const auto& avg_item : averages_map) {
    csv << avg_item.first << csv_sep;
  }
  csv << "Speedup (total)" << csv_sep << "Speedup (ohne Solver)" << std::endl;

  // outputs column values
  csv << refineLevel << csv_sep << comm.size() << csv_sep << numDofs << csv_sep << 0 << csv_sep;
  for (const auto& avg_item : averages_map) {
    long clock_count = avg_item.second;
    clock_count = long(comm.sum(clock_count) / double(scale_factor * numProce));
    csv << clock_count / double(datamaps_.size()) << csv_sep;
  }
  csv << "=I$2/I2" << csv_sep << "=SUM(E$2:G$2)/SUM(E2:G2)" << std::endl;
  csv.close();
} // OutputAveraged


void Profiler::setOutputdir(const std::string dir)
{
  output_dir_ = dir;
  Dune::Stuff::Common::testCreateDirectory(output_dir_);
}

void Profiler::outputTimings(const std::string csv) const
{
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  boost::filesystem::path dir(output_dir_);
  boost::filesystem::path filename = dir / (boost::format("%s_p%08d.csv") % csv % comm.rank()).str();
  boost::filesystem::ofstream out(filename);
  outputTimings(out);
  if (comm.rank() == 0) {
    boost::filesystem::path a_filename = dir / (boost::format("%s.csv") % csv).str();
    boost::filesystem::ofstream a_out(a_filename);
    outputTimingsAll(a_out);
  }
}

void Profiler::outputTimingsAll(std::ostream& out) const
{
  if (datamaps_.size() < 1)
    return;
  // csv header:
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  out << "run";
  for (const auto& section : datamaps_[0]) {
    out << csv_sep << section.first << "_avg" << csv_sep << section.first << "_sum";
  }
  int i = 0;
  for (const auto& datamap : datamaps_) {
    out << std::endl << i++;
    for (const auto& section : datamap) {
      auto val = section.second;
      auto sum = comm.sum(val);
      out << csv_sep << sum / float(comm.size()) << csv_sep << sum;
    }
  }
  out << std::endl;
}

void Profiler::outputTimings(std::ostream& out) const
{
  if (datamaps_.size() < 1)
    return;
  // csv header:
  out << "run";
  for (const auto& section : datamaps_[0]) {
    out << csv_sep << section.first;
  }
  int i = 0;
  for (const auto& datamap : datamaps_) {
    out << std::endl << i;
    for (const auto& section : datamap) {
      out << csv_sep << section.second;
    }
    out << std::endl;
  }
}

Profiler::Profiler()
  : csv_sep(",")
{
  reset(1);
  setOutputdir("./profiling");
}

Profiler::~Profiler()
{
}


} // namespace Common
} // namespace Stuff
} // namespace Dune
