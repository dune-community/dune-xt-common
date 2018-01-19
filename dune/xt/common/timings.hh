// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014 - 2017)
//   Rene Milk       (2009 - 2016, 2018)
//   Sven Kaulmann   (2011)

#ifndef DUNE_XT_COMMON_PROFILER_HH
#define DUNE_XT_COMMON_PROFILER_HH

#ifndef DUNE_XT_COMMON_DO_PROFILE
#define DUNE_XT_COMMON_DO_PROFILE 0
#endif

#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <memory>
#include <iostream>
#include <atomic>
#include <mutex>

#include <boost/noncopyable.hpp>
#include <boost/timer/timer.hpp>

#include <dune/common/unused.hh>
#include <dune/common/parallel/mpihelper.hh>

#include <dune/xt/common/parallel/threadmanager.hh>
#include <dune/xt/common/parallel/threadstorage.hh>

namespace Dune {
namespace XT {
namespace Common {

class Timings;

//! XT::Profiler global instance
Timings& timings();

//! wraps name, start- and end time for one timing section
struct TimingData
{
private:
  std::shared_ptr<boost::timer::cpu_timer> timer_;

public:
  std::string name;

  explicit TimingData(const std::string _name = "blank");

  void stop();

  typedef boost::timer::nanosecond_type TimeType;
  typedef std::array<TimeType, 3> DeltaType;

  /** \return array{wall,user,sys}: times elapsed since object construction in
   *milliseconds
   *  \note since typical resolutions for user+system time are 10-15ms the
   *nanosecond results are scaled accordingly
   **/
  DeltaType delta() const;
};

//! a utility class to time a limited scope of code
class ScopedTiming;

/** \brief simple inline timing class
   *  - User can set as many (even nested) named sections whose total (=system+user) time will be computed across all
   *    program instances.\n
   *  - Provides csv-conform output of process-averaged runtimes.
   **/
class Timings
{
  friend Timings& timings();

private:
  Timings();
  ~Timings();

  typedef std::map<std::string, std::pair<std::atomic<bool>, PerThreadValue<TimingData>>> KnownTimersMap;
  //! section name -> seconds
  typedef std::map<std::string, TimingData::DeltaType> DeltaMap;

  const TimingData& get_timing_data(std::string section_name) const;

public:
  void stop();

  //! set this to begin a named section
  void start(const std::string section_name);

  //! stop named section's counter
  long stop(const std::string section_name);

  //! set elapsed time back to 0 for section_name
  void reset(const std::string section_name);

  //! get runtime of section in milliseconds
  TimingData::TimeType walltime(const std::string section_name) const;
  //! get the full delta array
  TimingData::DeltaType delta(const std::string section_name) const;

  /** creates one file local to each MPI-rank (no global averaging)
   *  one single rank-0 file with all combined/averaged measures
   ***/
  void output_per_rank(const std::string csv_base) const;
  //! outputs walltime only w/o MPI-rank averaging
  void output_simple(std::ostream& out = std::cout) const;
  /** output all recorded measures
   * \note outputs average, min, max over all MPI processes associated to mpi_comm **/
  void output_all_measures(std::ostream& out = std::cout,
                           MPIHelper::MPICommunicator mpi_comm = Dune::MPIHelper::getCommunicator()) const;

  /// stops and resets all timers and data
  void reset();

  void set_outputdir(const std::string dir);

private:
  DeltaMap commited_deltas_;
  //! runtime tables etc go there
  std::string output_dir_;

  KnownTimersMap known_timers_map_;
  const std::string csv_sep_;
  std::mutex mutex_;

  static Timings& instance()
  {
    static Timings pf;
    return pf;
  }
};

//! global profiler object
inline Timings& timings()
{
  return Timings::instance();
}

class ScopedTiming : public boost::noncopyable
{
protected:
  const std::string section_name_;

public:
  explicit inline ScopedTiming(const std::string& section_name)
    : section_name_(section_name)
  {
    timings().start(section_name_);
  }

  inline ~ScopedTiming()
  {
    timings().stop(section_name_);
  }
};

struct OutputScopedTiming : public ScopedTiming
{
  OutputScopedTiming(const std::string& section_name, std::ostream& out);

  ~OutputScopedTiming();

protected:
  std::ostream& out_;
};

} // namespace Common
} // namespace XT
} // namespace Dune

#define DXTC_TIMINGS Dune::XT::Common::timings()

#if DUNE_XT_COMMON_DO_TIMING
#define DUNE_XT_COMMON_TIMING_SCOPE(section_name) Dune::XT::Common::ScopedTiming DXTC_UNUSED(timer)(section_name)
#else
#define DUNE_XT_COMMON_TIMING_SCOPE(section_name)
#endif

#endif // DUNE_XT_COMMON_PROFILER_HH
