#ifndef DUNE_STUFF_PROFILER_HH_INCLUDED
#define DUNE_STUFF_PROFILER_HH_INCLUDED


#include <dune/common/exceptions.hh>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "misc.hh"
#include "debug.hh"
#include "filesystem.hh"
#include "parametercontainer.hh"

//! wraps name, start- and end time for one timing section
struct TimingData
{
  clock_t start;
  clock_t end;
  std::string name;
  TimingData(const std::string _name, const clock_t _start)
    : start(_start)
    , end((clock_t)0.0)
    , name(_name){};
  TimingData()
    : start((clock_t)0.0)
    , end((clock_t)0.0)
    , name("blank"){};
};

/** \brief simple inline profiling class
 *
 *  - User can set as many (even nested) named sections whose total clock time will be computed across all program
 *instances.\n
 *  - Provides csv-conform output of process-averaged runtimes.
 **/
class Profiler
{
  friend Profiler& profiler();

protected:
  Profiler()
  {
    Reset(1);
  }
  ~Profiler()
  {
  }

  typedef std::vector<TimingData> TimingVector;
  typedef std::map<std::string, TimingVector> DataMap;
  typedef std::vector<DataMap> MapVector;

public:
  //! set this to begin a named section
  void StartTiming(const std::string section_name)
  {
    TimingData td(section_name, clock());
    if (m_cur_run_num >= m_timings.size()) {
      m_timings.push_back(DataMap());
      m_total_runs++;
    }
    (m_timings[m_cur_run_num])[section_name].push_back(td);
  }

  //! stop named section's counter
  void StopTiming(const std::string section_name)
  {
    assert(m_cur_run_num < m_timings.size());
    if ((m_timings[m_cur_run_num])[section_name].size() < 1)
      DUNE_THROW(Dune::RangeError, "trying to stop timer " << section_name << " that wasn't started\n");
    (m_timings[m_cur_run_num])[section_name].back().end = clock();
  }

  //! get runtime of section in seconds
  long GetTiming(const std::string section_name) const
  {
    std::cerr << "SEC " << section_name << std::endl;
    assert(m_cur_run_num < m_timings.size());
    const DataMap& data             = m_timings[m_cur_run_num];
    DataMap::const_iterator section = data.find(section_name);
    if (section == data.end()) {
      ASSERT_EXCEPTION(false, "no timer found: " + section_name);
      return -1;
    }
    return GetTiming(section->second);
  }
  long GetTiming(const TimingVector& timing_vector) const
  {
    TimingVector::const_iterator endIt = timing_vector.end();
    TimingVector::const_iterator it    = timing_vector.begin();
    long diff = 0;
    for (; it != endIt; ++it) {
      diff += it->end - it->start;
    }
    return long(diff / double(CLOCKS_PER_SEC));
  }

  /** output to currently pre-defined (csv) file, does not output individual run results, but average over all recorded
    *results
       * \param comm used to gather and average the runtime data over all processes
       * \tparam CollectiveCommunication should be Dune::CollectiveCommunication< MPI_Comm / double >
       **/
  template <class CollectiveCommunication>
  long OutputAveraged(CollectiveCommunication& comm, const int refineLevel, const long numDofs,
                      const double scale_factor = 1.0);

  /** output to \param filename
   * \param comm used to gather and average the runtime data over all processes
   * \tparam CollectiveCommunication should be Dune::CollectiveCommunication< MPI_Comm / double >
   **/
  template <class CollectiveCommunication, class InfoContainer>
  long OutputCommon(CollectiveCommunication& comm, InfoContainer& run_infos, std::string filename,
                    const double scale_factor = 1.0);

  //! default proxy for output
  template <class CollectiveCommunication, class InfoContainer>
  long Output(CollectiveCommunication& comm, InfoContainer& run_infos, const double scale_factor = 1.0);

  //! proxy for output of a map of runinfos
  template <class CollectiveCommunication, class InfoContainerMap>
  void OutputMap(CollectiveCommunication& comm, InfoContainerMap& run_infos_map, const double scale_factor = 1.0);

  /** call this with correct numRuns <b> before </b> starting any profiling
   *  if you're planning on doing more than one iteration of your code
   *  called once fromm ctor with numRuns=1
   **/
  void Reset(const int numRuns)
  {
    m_timings.clear();
    m_timings     = MapVector(numRuns, DataMap());
    m_total_runs  = numRuns;
    m_cur_run_num = 0;
    init_time_    = clock();
  }

  //! simple counter, usable to count how often a single piece of code is called
  void AddCount(const int num)
  {
    m_count[num] += 1;
  }

  //! call this after one iteration of your code has finished. increments current run number and puts new timing data
  //! into the vector
  void NextRun()
  {
    m_cur_run_num++;
  }

  //! a utility class to time a limited scope of code
  class ScopedTiming
  {
    const std::string section_name_;

  public:
    ScopedTiming(const std::string& section_name)
      : section_name_(section_name)
    {
      Profiler::instance().StartTiming(section_name_);
    }
    ~ScopedTiming()
    {
      Profiler::instance().StopTiming(section_name_);
    }
  };

protected:
  MapVector m_timings;
  unsigned int m_cur_run_num;
  unsigned int m_total_runs;
  // debug counter, only outputted in debug mode
  std::map<int, int> m_count;
  clock_t init_time_;

  static Profiler& instance()
  {
    static Profiler pf;
    return pf;
  }
};

template <class CollectiveCommunication>
long Profiler::OutputAveraged(CollectiveCommunication& comm, const int refineLevel, const long numDofs,
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
#endif

  Stuff::testCreateDirectory(Stuff::pathOnly(filename.str()));
  std::ofstream csv((filename.str()).c_str());

  typedef std::map<std::string, long> AvgMap;
  AvgMap averages;
  for (MapVector::const_iterator vit = m_timings.begin(); vit != m_timings.end(); ++vit) {
    for (DataMap::const_iterator it = vit->begin(); it != vit->end(); ++it) {
      averages[it->first] += GetTiming(it->second);
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
}

template <class CollectiveCommunication, class InfoContainer>
long Profiler::Output(CollectiveCommunication& comm, InfoContainer& run_infos, const double scale_factor)
{
  const int numProce = comm.size();

  std::ostringstream filename;
  filename << Parameters().getParam("fem.io.datadir", std::string(".")) << "/prof_p" << numProce << ".csv";
  filename.flush();
  return OutputCommon(comm, run_infos, filename.str(), scale_factor);
}

template <class CollectiveCommunication, class InfoContainerMap>
void Profiler::OutputMap(CollectiveCommunication& comm, InfoContainerMap& run_infos_map, const double scale_factor)
{
  std::string dir(Parameters().getParam("fem.io.datadir", std::string(".")));
  BOOST_FOREACH (typename InfoContainerMap::value_type el, run_infos_map) {
    OutputCommon(
        comm, el.second, (boost::format("%s/prof_p%d_ref%s.csv") % dir % comm.size() % el.first).str(), scale_factor);
  }
}

template <class CollectiveCommunication, class InfoContainer>
long Profiler::OutputCommon(CollectiveCommunication& comm, InfoContainer& run_infos, std::string filename,
                            const double scale_factor)
{
  const int numProce = comm.size();

  if (comm.rank() == 0)
    std::cout << "Profiling info in: " << filename.c_str() << std::endl;

#ifndef NDEBUG
  for (std::map<int, int>::const_iterator it = m_count.begin(); it != m_count.end(); ++it) {
    std::cout << "proc " << comm.rank() << " bId " << it->first << " count " << it->second << std::endl;
  }
#endif

  Stuff::testCreateDirectory(Stuff::pathOnly(filename));
  std::ofstream csv(filename.c_str());

  // outputs column names
  csv << "refine\t"
      << "processes\t"
      << "numDofs\t"
      << "L2_error\t";
  for (DataMap::const_iterator it = m_timings[0].begin(); it != m_timings[0].end(); ++it) {
    csv << it->first << "\t";
  }
  csv << "Relative_total_time" << std::endl;

  // outputs column values

  MapVector::const_iterator ti_it = m_timings.begin();
  int idx = 0;
  assert(run_infos.size() >= m_timings.size());
  for (; ti_it != m_timings.end(); ++ti_it) {
    RunInfo info = run_infos[idx];
    csv << boost::format("%d\t%d\t%d\t%e\t") % info.refine_level % comm.size() % info.codim0
               % (info.L2Errors.size() ? info.L2Errors[0] : double(-1));

    const DataMap& data_map = *ti_it;
    for (DataMap::const_iterator it = data_map.begin(); it != data_map.end(); ++it) {
      long clock_count = GetTiming(it->second);
      clock_count      = long(comm.sum(clock_count) / double(scale_factor * numProce));
      csv << clock_count << "\t";
    }
    csv << "=1/I$2*I" << Stuff::toString(idx + 2) << std::endl;

    idx++;
  }

  csv.close();

  return long((clock() - init_time_) / double(CLOCKS_PER_SEC * scale_factor));
}

//! global profiler object (for legacy code compat this is outside NS Stuff)
Profiler& profiler()
{
  return Profiler::instance();
}

#endif // DUNE_STUFF_PROFILER_HH_INCLUDED
