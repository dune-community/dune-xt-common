#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

void Profiler::startTiming(const std::string section_name)
{
  if (m_cur_run_num >= m_timings.size()) {
    m_timings.push_back(DataMap());
    m_total_runs++;
  }

  KnownTimersMap::iterator section = known_timers_map_.find(section_name);
  if (section != known_timers_map_.end()) {
    if (section->second.first) // timer currently running
      return;

    section->second.first  = true; // set active, start with new
    section->second.second = TimingData(section_name, clock());
  } else {
    // init new section
    known_timers_map_[section_name] = std::make_pair(true, TimingData(section_name, clock()));
  }
} // StartTiming

void Profiler::stopTiming(const std::string section_name)
{
  assert(m_cur_run_num < m_timings.size());
  if (known_timers_map_.find(section_name) == known_timers_map_.end())
    DUNE_THROW(Dune::RangeError, "trying to stop timer " << section_name << " that wasn't started\n");

  known_timers_map_[section_name].first      = false;
  known_timers_map_[section_name].second.end = clock();
  DataMap& current_data = m_timings[m_cur_run_num];
  if (current_data.find(section_name) == current_data.end())
    current_data[section_name] = known_timers_map_[section_name].second.delta();
  else
    current_data[section_name] += known_timers_map_[section_name].second.delta();
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
  return long(section->second / double(CLOCKS_PER_SEC));
} // GetTiming


void Profiler::reset(const int numRuns)
{
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

template <class CollectiveCommunication>
long Profiler::outputAveraged(CollectiveCommunication& comm, const int refineLevel, const long numDofs,
                              const double scale_factor)
{
  const int numProce = comm.size();

  std::ostringstream filename;

  filename << "p" << numProce << "_refinelvl_" << refineLevel << ".csv";
  filename.flush();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << (filename.str()).c_str() << std::endl;

#ifndef NDEBUG
  for (std::map<int, int>::const_iterator it = m_count.begin(); it != m_count.end(); ++it) {
    std::cout << "proc " << comm.rank() << " bId " << it->first << " count " << it->second << std::endl;
  }
#endif // ifndef NDEBUG

  Dune::Stuff::Common::Filesystem::testCreateDirectory(Dune::Stuff::Common::Filesystem::pathOnly(filename.str()));
  std::ofstream csv((filename.str()).c_str());

  typedef std::map<std::string, long> AvgMap;
  AvgMap averages;
  for (MapVector::const_iterator vit = m_timings.begin(); vit != m_timings.end(); ++vit) {
    for (DataMap::const_iterator it = vit->begin(); it != vit->end(); ++it) {
      //! this used to be GetTiming( it->second ), which is only valid thru an implicit and wrong conversion..
      averages[it->first] += getTiming(it->first);
    }
  }

  // outputs column names
  csv << "refine,"
      << "processes,"
      << "numDofs,"
      << "L1 error,";
  for (AvgMap::const_iterator it = averages.begin(); it != averages.end(); ++it) {
    csv << it->first << ",";
  }
  csv << "Speedup (total); Speedup (ohne Solver)" << std::endl;

  // outputs column values
  csv << refineLevel << "," << comm.size() << "," << numDofs << "," << 0 << ","; //! FIXME
  for (AvgMap::const_iterator it = averages.begin(); it != averages.end(); ++it) {
    long clock_count = it->second;
    clock_count = long(comm.sum(clock_count) / double(scale_factor * numProce));
    csv << clock_count / double(m_total_runs) << ",";
  }
  csv << "=I$2/I2,"
      << "=SUM(E$2:G$2)/SUM(E2:G2)" << std::endl;

  csv.close();

  return long((clock() - init_time_) / double(CLOCKS_PER_SEC * scale_factor));
} // OutputAveraged

template <class CollectiveCommunication, class InfoContainer>
long Profiler::output(CollectiveCommunication& comm, InfoContainer& run_infos, const double scale_factor)
{
  const int numProce = comm.size();

  std::ostringstream filename;

  filename << Dune::Stuff::Common::Parameter::Parameters().getParam("fem.io.datadir", std::string(".")) << "/prof_p"
           << numProce << ".csv";
  filename.flush();
  return OutputCommon(comm, run_infos, filename.str(), scale_factor);
} // Output

template <class CollectiveCommunication, class InfoContainerMap>
void Profiler::outputMap(CollectiveCommunication& comm, InfoContainerMap& run_infos_map, const double scale_factor)
{
  std::string dir(Dune::Stuff::Common::Parameter::Parameters().getParam("fem.io.datadir", std::string(".")));

  BOOST_FOREACH (typename InfoContainerMap::value_type el, run_infos_map) {
    OutputCommon(
        comm, el.second, (boost::format("%s/prof_p%d_ref%s.csv") % dir % comm.size() % el.first).str(), scale_factor);
  }
} // OutputMap

template <class CollectiveCommunication, class InfoContainer>
long Profiler::outputCommon(CollectiveCommunication& comm, InfoContainer& run_infos, std::string filename,
                            const double scale_factor)
{
  const int numProce = comm.size();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << filename.c_str() << std::endl;

#ifndef NDEBUG
  for (std::map<int, int>::const_iterator it = m_count.begin(); it != m_count.end(); ++it) {
    std::cout << "proc " << comm.rank() << " bId " << it->first << " count " << it->second << std::endl;
  }
#endif // ifndef NDEBUG

  Dune::Stuff::Common::Filesystem::testCreateDirectory(Dune::Stuff::Common::Filesystem::pathOnly(filename));
  std::ofstream csv(filename.c_str());

  // outputs column names
  csv << "refine,"
      << "processes,"
      << "numDofs,"
      << "L2_error,";
  for (DataMap::const_iterator it = m_timings[0].begin(); it != m_timings[0].end(); ++it) {
    csv << it->first << ",";
  }
  csv << "Relative_total_time,compiler" << std::endl;

  // outputs column values

  MapVector::const_iterator ti_it = m_timings.begin();
  int idx = 0;
  assert(run_infos.size() >= m_timings.size());
  for (; ti_it != m_timings.end(); ++ti_it) {
    Dune::Stuff::Common::RunInfo info = run_infos[idx];
    csv << boost::format("%d,%d,%d,%e,") % info.refine_level % comm.size() % info.codim0
               % (info.L2Errors.size() ? info.L2Errors[0] : double(-1));

    const DataMap& data_map = *ti_it;
    for (DataMap::const_iterator it = data_map.begin(); it != data_map.end(); ++it) {
      long clock_count = getTiming(it->first, idx);
      clock_count      = long(comm.sum(clock_count) / double(scale_factor * numProce));
      csv << clock_count << ",";
    }
    csv << boost::format("=1/I$2*I%d,%s\n") % (idx + 2) % BOOST_COMPILER;

    idx++;
  }

  csv.close();

  return long((clock() - init_time_) / double(CLOCKS_PER_SEC * scale_factor));
} // OutputCommon

} // namespace Common
} // namespace Stuff
} // namespace Dune
