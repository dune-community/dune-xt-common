#include <dune/common/mpihelper.hh>

#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
namespace Common {

void Profiler::startTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + String::convertTo(i);
  startTiming(section);
}

long Profiler::stopTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + String::convertTo(i);
  return stopTiming(section);
}

long Profiler::getTiming(const std::string section_name, const int i) const
{
  const std::string section = section_name + String::convertTo(i);
  return getTiming(section);
}

void Profiler::resetTiming(const std::string section_name, const int i)
{
  const std::string section = section_name + String::convertTo(i);
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
    if (timer_it != known_timers_map_.end())
      return timer_it->second.second.delta();
    ASSERT_EXCEPTION(false, "no timer found: " + section_name);
    return -1;
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

void Profiler::output(const Profiler::InfoContainer& run_infos, const double scale_factor) const
{
  const auto& comm   = Dune::MPIHelper::getCollectiveCommunication();
  const int numProce = comm.size();

  boost::filesystem::path filename(output_dir_);
  filename /= (boost::format("prof_p%d.csv") % numProce).str();
  outputCommon(run_infos, filename, scale_factor);
} // Output

void Profiler::outputMap(const Profiler::InfoContainerMap& run_infos_map, const double scale_factor) const
{
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  for (const auto& el : run_infos_map) {
    boost::filesystem::path filename(output_dir_);
    filename /= (boost::format("prof_p%d_ref%s.csv") % comm.size() % el.first).str();
    outputCommon(el.second, filename, scale_factor);
  }
} // OutputMap

void Profiler::outputCommon(const Profiler::InfoContainer& run_infos, const boost::filesystem::path& filename,
                            const double scale_factor) const
{
  const auto& comm   = Dune::MPIHelper::getCollectiveCommunication();
  const int numProce = comm.size();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << filename << std::endl;

#ifndef NDEBUG
  for (const auto& count : counters_) {
    std::cout << "proc " << comm.rank() << " bId " << count.first << " count " << count.second << std::endl;
  }
#endif // ifndef NDEBUG

  boost::filesystem::ofstream csv(filename);
  // outputs column names
  if (run_infos.size()) {
    csv << "refine" << csv_sep << "processes" << csv_sep << "numDofs" << csv_sep << "L2_error" << csv_sep;
    for (Datamap::const_iterator it = datamaps_[0].begin(); it != datamaps_[0].end(); ++it) {
      csv << it->first << csv_sep;
    }
  }
  csv << "Relative_total_time" << csv_sep << "compiler" << std::endl;

  // outputs column values
  int idx = 0;
  for (const Datamap& data_map : datamaps_) {
    if (idx < run_infos.size()) {
      const Dune::Stuff::Common::RunInfo& info = run_infos[idx];
      csv << boost::format("%d%s%d%s%d%s%e,") % info.refine_level % csv_sep % comm.size() % csv_sep % info.codim0
                 % csv_sep % (info.L2Errors.size() ? info.L2Errors[0] : double(-1));
    }

    for (Datamap::const_iterator it = data_map.begin(); it != data_map.end(); ++it) {
      long clock_count = getTimingIdx(it->first, idx);
      clock_count      = long(comm.sum(clock_count) / double(scale_factor * numProce));
      csv << clock_count << csv_sep;
    }
    csv << boost::format("=1/I$2*I%d%s%s\n") % (idx + 2) % csv_sep % BOOST_COMPILER;

    idx++;
  }
  csv.close();
} // OutputCommon

void Profiler::setOutputdir(const std::string dir)
{
  output_dir_ = dir;
  Dune::Stuff::Common::Filesystem::testCreateDirectory(output_dir_);
}

void Profiler::outputTimings(const std::string csv) const
{
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  boost::filesystem::path filename(output_dir_);
  filename /= (boost::format("timings_p%08d.csv") % comm.rank()).str();
  boost::filesystem::ofstream out(filename);
  outputTimings(out);
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
