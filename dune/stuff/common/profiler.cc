// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#include "config.h"
#include "profiler.hh"

#include <dune/common/version.hh>
#include <dune/stuff/aliases.hh>


#if HAVE_LIKWID && ENABLE_PERFMON
#include <likwid.h>
#define DSC_LIKWID_BEGIN_SECTION(name) LIKWID_MARKER_START(name.c_str());
#define DSC_LIKWID_END_SECTION(name) LIKWID_MARKER_STOP(name.c_str());
#define DSC_LIKWID_INIT LIKWID_MARKER_INIT
#define DSC_LIKWID_CLOSE LIKWID_MARKER_CLOSE
#else
#define DSC_LIKWID_BEGIN_SECTION(name)
#define DSC_LIKWID_END_SECTION(name)
#define DSC_LIKWID_INIT
#define DSC_LIKWID_CLOSE
#endif

#if DUNE_VERSION_NEWER(DUNE_COMMON, 2, 3)
#include <dune/common/parallel/mpihelper.hh>
#else
#include <dune/common/mpihelper.hh>
#endif

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/parallel/threadmanager.hh>

#include <map>
#include <string>

#include <dune/stuff/common/disable_warnings.hh>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/config.hpp>
#include <boost/timer/timer.hpp>
#include <dune/stuff/common/reenable_warnings.hh>


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

TimingData::DeltaType TimingData::delta() const
{
  const auto scale   = 1.0 / double(boost::timer::nanosecond_type(1e6));
  const auto elapsed = timer_->elapsed();
  const auto cast = [=](double var) { return static_cast<typename TimingData::DeltaType::value_type>(var); };
  return {{cast(elapsed.wall * scale), cast(elapsed.user * scale), cast(elapsed.system * scale)}};
}

void Profiler::resetTiming(const std::string section_name)
{
  try {
    stopTiming(section_name);
  } catch (Dune::RangeError) {
    // ok, timer simply wasn't running
  }
  Datamap& current_data      = datamaps_[current_run_number_];
  current_data[section_name] = {{0, 0, 0}};
}

void Profiler::startTiming(const std::string section_name)
{
  std::lock_guard<std::mutex> lock(mutex_);
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
  DSC_LIKWID_BEGIN_SECTION(section_name)
} // StartTiming

long Profiler::stopTiming(const std::string section_name)
{
  DSC_LIKWID_END_SECTION(section_name)
  assert(current_run_number_ < datamaps_.size());
  if (known_timers_map_.find(section_name) == known_timers_map_.end())
    DUNE_THROW(Dune::RangeError, "trying to stop timer " << section_name << " that wasn't started\n");

  known_timers_map_[section_name].first = false; // marks as not running
  TimingData& timing = *(known_timers_map_[section_name].second);
  timing.stop();
  const auto delta      = timing.delta();
  Datamap& current_data = datamaps_[current_run_number_];
  if (current_data.find(section_name) == current_data.end())
    current_data[section_name] = delta;
  else {
    for (auto i : valueRange(delta.size()))
      current_data[section_name][i] += delta[i];
  }
  return delta[0];
} // StopTiming

long Profiler::getTiming(const std::string section_name) const
{
  return get_delta(section_name)[0];
}

TimingData::DeltaType Profiler::get_delta(const std::string section_name) const
{
  assert(current_run_number_ < datamaps_.size());
  return getTimingIdx(section_name, current_run_number_);
}

TimingData::DeltaType Profiler::getTimingIdx(const std::string section_name, const size_t run_number) const
{
  assert(run_number < datamaps_.size());
  const Datamap& data             = datamaps_[run_number];
  Datamap::const_iterator section = data.find(section_name);
  if (section == data.end()) {
    // timer might still be running
    const auto& timer_it = known_timers_map_.find(section_name);
    if (timer_it == known_timers_map_.end())
      DUNE_THROW(Dune::InvalidStateException, "no timer found: " + section_name);
    return timer_it->second.second->delta();
  }
  return section->second;
} // GetTiming


void Profiler::reset(const size_t numRuns)
{
  if (!(numRuns > 0))
    DUNE_THROW(Dune::RangeError, "preparing the profiler for 0 runs is moronic");
  datamaps_.clear();
  datamaps_           = DatamapVector(numRuns, Datamap());
  current_run_number_ = 0;
} // Reset

void Profiler::addCount(const size_t num)
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
  csv << "refine" << csv_sep_ << "processes" << csv_sep_ << "numDofs" << csv_sep_ << "L1 error" << csv_sep_;
  for (const auto& avg_item : averages_map) {
    csv << avg_item.first << csv_sep_;
  }
  csv << "Speedup (total)" << csv_sep_ << "Speedup (ohne Solver)" << std::endl;

  // outputs column values
  csv << refineLevel << csv_sep_ << comm.size() << csv_sep_ << numDofs << csv_sep_ << 0 << csv_sep_;
  for (const auto& avg_item : averages_map) {
    long clock_count = avg_item.second;
    clock_count = long(comm.sum(clock_count) / double(scale_factor * numProce));
    csv << clock_count / double(datamaps_.size()) << csv_sep_;
  }
  csv << "=I$2/I2" << csv_sep_ << "=SUM(E$2:G$2)/SUM(E2:G2)" << std::endl;
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
  boost::filesystem::path a_filename = dir / (boost::format("%s.csv") % csv).str();
  boost::filesystem::ofstream a_out(a_filename);
  outputTimingsAll(a_out);
}

void Profiler::outputTimingsAll(std::ostream& out) const
{
  if (datamaps_.size() < 1)
    return;
  // csv header:
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();

  std::stringstream stash;

  stash << "run" << csv_sep_ << "threads" << csv_sep_ << "ranks";
  for (const auto& section : datamaps_[0]) {
    stash << csv_sep_ << section.first << "_avg_mix" << csv_sep_ << section.first << "_max_mix" << csv_sep_
          << section.first << "_avg_usr" << csv_sep_ << section.first << "_max_usr" << csv_sep_ << section.first
          << "_avg_wall" << csv_sep_ << section.first << "_max_wall" << csv_sep_ << section.first << "_avg_sys"
          << csv_sep_ << section.first << "_max_sys";
  }
  int i             = 0;
  const auto weight = 1 / double(comm.size());
  for (const auto& datamap : datamaps_) {
    stash << std::endl << i++ << csv_sep_ << DS::threadManager().max_threads() << csv_sep_ << comm.size();
    for (const auto& section : datamap) {
      const auto timings  = section.second;
      auto wall           = timings[1];
      auto usr            = timings[2];
      auto sys            = timings[3];
      auto mix            = timings[0];
      const auto wall_sum = comm.sum(wall);
      const auto wall_max = comm.max(wall);
      const auto usr_sum  = comm.sum(usr);
      const auto usr_max  = comm.max(usr);
      const auto sys_sum  = comm.sum(sys);
      const auto sys_max  = comm.max(sys);
      const auto mix_sum  = comm.sum(mix);
      const auto mix_max  = comm.max(mix);
      stash << csv_sep_ << mix_sum * weight << csv_sep_ << mix_max << csv_sep_ << usr_sum * weight << csv_sep_
            << usr_max << csv_sep_ << wall_sum * weight << csv_sep_ << wall_max << csv_sep_ << sys_sum * weight
            << csv_sep_ << sys_max;
    }
  }
  stash << std::endl;
  if (comm.rank() == 0)
    out << stash.str();
}

void Profiler::outputTimings(std::ostream& out) const
{
  if (datamaps_.size() < 1)
    return;
  // csv header:
  out << "run";
  for (const auto& section : datamaps_[0]) {
    out << csv_sep_ << section.first;
  }
  size_t i = 0;
  for (const auto& datamap : datamaps_) {
    out << std::endl << i;
    for (const auto& section : datamap) {
      out << csv_sep_ << section.second[0];
    }
    out << std::endl;
  }
}

Profiler::Profiler()
  : csv_sep_(",")
{
  DSC_LIKWID_INIT;
  reset(1);
  setOutputdir("./profiling");
}

Profiler::~Profiler()
{
  DSC_LIKWID_CLOSE;
}

OutputScopedTiming::OutputScopedTiming(const std::string& section_name, std::ostream& out)
  : ScopedTiming(section_name)
  , out_(out)
{
}

OutputScopedTiming::~OutputScopedTiming()
{
  const auto duration = profiler().stopTiming(section_name_);
  out_ << "Executing " << section_name_ << " took " << duration / 1000.f << "s\n";
}


} // namespace Common
} // namespace Stuff
} // namespace Dune
