#include <dune/common/mpihelper.hh>

#include <map>
#include <string>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

void Profiler::startTiming(const std::string section_name)
{
  if (m_cur_run_num >= m_timings.size()) {
    m_timings.push_back(DataMap());
    m_total_runs++;
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

void Profiler::stopTiming(const std::string section_name)
{
  assert(m_cur_run_num < m_timings.size());
  if (known_timers_map_.find(section_name) == known_timers_map_.end())
    DUNE_THROW(Dune::RangeError, "trying to stop timer " << section_name << " that wasn't started\n");

  known_timers_map_[section_name].first = false; // marks as not running
  TimingData& timing = known_timers_map_[section_name].second;
  timing.stop();
  long long delta       = timing.delta();
  DataMap& current_data = m_timings[m_cur_run_num];
  if (current_data.find(section_name) == current_data.end())
    current_data[section_name] = delta;
  else
    current_data[section_name] += delta;
} // StopTiming

long Profiler::getTiming(const std::string section_name) const
{
  assert(m_cur_run_num < m_timings.size());
  return getTiming(section_name, m_cur_run_num);
}

long Profiler::getTiming(const std::string section_name, const int run_number) const
{
  assert(run_number < int(m_timings.size()));
  const DataMap& data             = m_timings[run_number];
  DataMap::const_iterator section = data.find(section_name);
  if (section == data.end()) {
    ASSERT_EXCEPTION(false, "no timer found: " + section_name);
    return -1;
  }
  return section->second;
} // GetTiming


void Profiler::reset(const int numRuns)
{
  if (!(numRuns > 0))
    DUNE_THROW(Dune::RangeError, "preparing the profiler for 0 runs is moronic");
  m_timings.clear();
  m_timings     = MapVector(numRuns, DataMap());
  m_total_runs  = numRuns;
  m_cur_run_num = 0;
  init_time_    = clock();
} // Reset

void Profiler::addCount(const int num)
{
  m_count[num] += 1;
}

void Profiler::nextRun()
{
  m_cur_run_num++;
}


void Profiler::outputAveraged(const int refineLevel, const long numDofs, const double scale_factor) const
{
  const auto& comm   = Dune::MPIHelper::getCollectiveCommunication();
  const int numProce = comm.size();

  boost::format csv_name("p%d_refinelvl_%d.csv");
  csv_name % numProce % refineLevel;
  boost::filesystem::path filename(m_output_dir);
  filename /= csv_name.str();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << filename << std::endl;

#ifndef NDEBUG
  for (const auto& count : m_count) {
    std::cout << "proc " << comm.rank() << " bId " << count.first << " count " << count.second << std::endl;
  }
#endif // ifndef NDEBUG

  boost::filesystem::ofstream csv(filename);

  std::map<std::string, long> averages_map;
  for (const auto& datamap : m_timings) {
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
    csv << clock_count / double(m_total_runs) << csv_sep;
  }
  csv << "=I$2/I2" << csv_sep << "=SUM(E$2:G$2)/SUM(E2:G2)" << std::endl;
  csv.close();
} // OutputAveraged

void Profiler::output(const Profiler::InfoContainer& run_infos, const double scale_factor) const
{
  const auto& comm   = Dune::MPIHelper::getCollectiveCommunication();
  const int numProce = comm.size();

  boost::filesystem::path filename(m_output_dir);
  filename /= (boost::format("prof_p%d.csv") % numProce).str();
  outputCommon(run_infos, filename, scale_factor);
} // Output

void Profiler::outputMap(const Profiler::InfoContainerMap& run_infos_map, const double scale_factor) const
{
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  for (const auto& el : run_infos_map) {
    boost::filesystem::path filename(m_output_dir);
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
  for (const auto& count : m_count) {
    std::cout << "proc " << comm.rank() << " bId " << count.first << " count " << count.second << std::endl;
  }
#endif // ifndef NDEBUG

  boost::filesystem::ofstream csv(filename);
  // outputs column names
  csv << "refine" << csv_sep << "processes" << csv_sep << "numDofs" << csv_sep << "L2_error" << csv_sep;
  for (DataMap::const_iterator it = m_timings[0].begin(); it != m_timings[0].end(); ++it) {
    csv << it->first << csv_sep;
  }
  csv << "Relative_total_time" << csv_sep << "compiler" << std::endl;

  // outputs column values
  int idx = 0;
  assert(run_infos.size() >= m_timings.size());
  for (const DataMap& data_map : m_timings) {
    const Dune::Stuff::Common::RunInfo& info = run_infos[idx];
    csv << boost::format("%d%s%d%s%d%s%e,") % info.refine_level % csv_sep % comm.size() % csv_sep % info.codim0
               % csv_sep % (info.L2Errors.size() ? info.L2Errors[0] : double(-1));

    for (DataMap::const_iterator it = data_map.begin(); it != data_map.end(); ++it) {
      long clock_count = getTiming(it->first, idx);
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
  m_output_dir = dir;
  Dune::Stuff::Common::Filesystem::testCreateDirectory(m_output_dir);
}

void Profiler::outputTimings(const std::string csv) const
{
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  boost::filesystem::path filename(m_output_dir);
  filename /= (boost::format("timings_p%08d.csv") % comm.rank()).str();
  boost::filesystem::ofstream out(filename);
  outputTimings(out);
}

void Profiler::outputTimings(std::ostream& out) const
{
  if (m_timings.size() < 1)
    return;
  // csv header:
  out << "run";
  for (const auto& section : m_timings[0]) {
    out << csv_sep << section.first;
  }
  int i = 0;
  for (const auto& datamap : m_timings) {
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

} // namespace Common
} // namespace Stuff
} // namespace Dune
