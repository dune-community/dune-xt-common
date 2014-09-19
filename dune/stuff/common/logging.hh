// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

/**
   *  \file logging.hh
   *  \brief  logging
   **/
#ifndef DUNE_STUFF_COMMON_LOGGING_HH
#define DUNE_STUFF_COMMON_LOGGING_HH

#include <map>
#include <string>

#include <fstream>
#include <ostream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/parallel/mpihelper.hh>
#include <dune/common/timer.hh>
#include <dune/stuff/common/reenable_warnings.hh>

#include "logstreams.hh"
#include "color.hh"

namespace Dune {
namespace Stuff {
namespace Common {


class Logging;
Logging& Logger();


/** \brief handles all logging
  **/
class Logging
{
private:
  Logging();
  //! cleanup stream and flag containers
  void deinit();

public:
  ~Logging();

  /** \brief setup loglevel, logfilename
     *  \param logflags any OR'd combination of flags
     *  \param logfile filename for log, can contain paths, but creation will fail if dir is non-existant
     **/
  void create(int logflags = (LOG_FILE | LOG_CONSOLE | LOG_ERROR), const std::string logfile = "dune_stuff_log",
              const std::string datadir = "data", const std::string _logdir = std::string("log"));

  //! \attention This will probably not do wht we want it to!
  void setPrefix(std::string prefix);
  void setStreamFlags(int streamID, int flags);
  int getStreamFlags(int streamID) const;

  /** \name forwarded Log functions
     * \{
     */
  template <class T>
  void log(T c, int streamID)
  {
    getStream(streamID) << c;
  } // Log

  /** \}
     */

  LogStream& getStream(int streamId);
  LogStream& error()
  {
    return getStream(LOG_ERROR);
  }
  LogStream& info()
  {
    return getStream(LOG_INFO);
  }
  LogStream& debug()
  {
    return getStream(LOG_DEBUG);
  }
  LogStream& devnull()
  {
    return emptyLogStream_;
  }

  //! flush all active streams
  void flush();
  //! creates a new LogStream with given id
  int addStream(int flags);

  //! re-enable all logging below given priority level
  void resume(LogStream::PriorityType prio = LogStream::default_suspend_priority);
  //! (temporarily) disable all logging below given priority level
  void suspend(LogStream::PriorityType prio = LogStream::default_suspend_priority);

  struct SuspendLocal
  {
    LogStream::PriorityType prio_;
    SuspendLocal(LogStream::PriorityType prio = LogStream::default_suspend_priority)
      : prio_(prio)
    {
      Logger().suspend(prio_);
    }

    ~SuspendLocal()
    {
      Logger().resume(prio_);
    }
  };

  struct ResumeLocal
  {
    LogStream::PriorityType prio_;
    ResumeLocal(LogStream::PriorityType prio = LogStream::default_suspend_priority)
      : prio_(prio)
    {
      Logger().resume(prio_);
    }

    ~ResumeLocal()
    {
      Logger().suspend(prio_);
    }
  };

private:
  boost::filesystem::path filename_;
  boost::filesystem::path filenameWoTime_;
  boost::filesystem::ofstream logfile_;
  typedef std::map<int, int> FlagMap;
  FlagMap flagmap_;
  typedef std::map<int, std::unique_ptr<LogStream>> StreamMap;
  StreamMap streammap_;
  typedef std::vector<int> IdVec;
  IdVec streamIDs_;
  int logflags_;
  EmptyLogStream emptyLogStream_;

  friend Logging& Logger();
  // satisfy stricter warnings wrt copying
  Logging(const Logging&) = delete;
  Logging& operator=(const Logging&) = delete;
};

//! global Logging instance
inline Logging& Logger()
{
  static Logging log;
  return log;
}


//! global timer instance
inline Timer& GlobalTimer()
{
  static Timer timer;
  return timer;
}


std::string elapsed_time();


} // namespace Common
} // namespace Stuff
} // namespace Dune


#define DSC_LOG Dune::Stuff::Common::Logger()
#define DSC_LOG_INFO DSC_LOG.info()
#define DSC_LOG_DEBUG DSC_LOG.debug()
#define DSC_LOG_ERROR DSC_LOG.error()
#define DSC_LOG_DEVNULL DSC_LOG.devnull()

#define DSC_LOG_INFO_0 (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.info() : DSC_LOG.devnull())
#define DSC_LOG_DEBUG_0                                                                                                \
  (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.debug() : DSC_LOG.devnull())
#define DSC_LOG_ERROR_0                                                                                                \
  (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.error() : DSC_LOG.devnull())

#define DSC_LOG_INFO_TIMED(str_id)                                                                                     \
  DSC_LOG_INFO << Dune::Stuff::Common::elapsed_time() << Dune::Stuff::Common::bold_white(str_id) << ": "
#define DSC_LOG_DEBUG_TIMED(str_id)                                                                                    \
  DSC_LOG_DEBUG << Dune::Stuff::Common::elapsed_time() << Dune::Stuff::Common::bold_white(str_id) << ": "


#endif // DUNE_STUFF_COMMON_LOGGING_HH
