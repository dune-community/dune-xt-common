// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012, 2014 - 2015)
//   Rene Milk       (2009 - 2015)
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
#include <mutex>

#include <boost/noncopyable.hpp>
#include <boost/timer/timer.hpp>

#include <dune/common/unused.hh>

#include <dune/xt/common/parallel/threadmanager.hh>
#include <dune/xt/common/parallel/threadstorage.hh>

namespace Dune {
namespace XT {
namespace Common {

class Profiler;

//! XT::Profiler global instance
Profiler& profiler();

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

  /** \return time elapsed since object construction in milliseconds
   *  \note since typical resolutions for user+system time are 10-15ms the nanosecond results are scaled accordingly
   **/
  DeltaType delta() const;
};

//! a utility class to time a limited scope of code
class ScopedTiming;

/** \brief simple inline profiling class
   *  - User can set as many (even nested) named sections whose total (=system+user) time will be computed across all
  *program
   * instances.\n
   *  - Provides csv-conform output of process-averaged runtimes.
   **/
class Profiler
{
  friend Profiler& profiler();

private:
  Profiler();
  ~Profiler();

  typedef std::map<std::string, std::pair<bool, PerThreadValue<TimingData>>> KnownTimersMap;
  //! section name -> seconds
  typedef std::map<std::string, TimingData::DeltaType> Datamap;
  //! "Run idx" -> Datamap = section name -> seconds
  typedef std::vector<Datamap> DatamapVector;

  //! get runtime of section in run run_number in milliseconds
  TimingData::DeltaType getTimingIdx(const std::string section_name, const size_t run_number) const;

public:
  void stopAll();

  //! set this to begin a named section
  void startTiming(const std::string section_name);

  //! stop named section's counter
  long stopTiming(const std::string section_name);

  //! set elapsed time back to 0 for section_name
  void resetTiming(const std::string section_name);

  //! get runtime of section in current run in milliseconds
  long getTiming(const std::string section_name) const;
  TimingData::DeltaType get_delta(const std::string section_name) const;

  /** output to currently pre-defined (csv) file, does not output individual run results, but average over all recorded
   * results
     **/
  void outputAveraged(const int refineLevel, const long numDofs, const double scale_factor = 1.0) const;

  //! file-output the named sections only
  void outputTimings(const std::string filename) const;
  void outputTimings(std::ostream& out = std::cout) const;
  void outputTimingsAll(std::ostream& out = std::cout) const;

  /** call this with correct numRuns <b> before </b> starting any profiling
     *  if you're planning on doing more than one iteration of your code
     *  called once fromm ctor with numRuns=1
     **/
  void reset(const size_t numRuns);

  //! simple counter, usable to count how often a single piece of code is called
  void addCount(const size_t num);

  //! call this after one iteration of your code has finished. increments current run number and puts new timing data
  //! into the vector
  void nextRun();

  void setOutputdir(const std::string dir);

private:
  DatamapVector datamaps_;
  size_t current_run_number_;
  //! runtime tables etc go there
  std::string output_dir_;
  // debug counter, only outputted in debug mode
  std::map<size_t, size_t> counters_;

  KnownTimersMap known_timers_map_;
  const std::string csv_sep_;
  std::mutex mutex_;

  static Profiler& instance()
  {
    static Profiler pf;
    return pf;
  }
};

//! global profiler object
inline Profiler& profiler()
{
  return Profiler::instance();
}

class ScopedTiming : public boost::noncopyable
{
protected:
  const std::string section_name_;

public:
  inline ScopedTiming(const std::string& section_name)
    : section_name_(section_name)
  {
    profiler().startTiming(section_name_);
  }

  inline ~ScopedTiming()
  {
    profiler().stopTiming(section_name_);
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

#define DXTC_PROFILER profiler()

#if DUNE_XT_COMMON_DO_PROFILE
#define DUNE_XT_COMMON_PROFILE_SCOPE(section_name) ScopedTiming DXTC_UNUSED(timer)(section_name)
#else
#define DUNE_XT_COMMON_PROFILE_SCOPE(section_name)
#endif

#endif // DUNE_XT_COMMON_PROFILER_HH
