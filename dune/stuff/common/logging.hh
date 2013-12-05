/**
   *  \file logging.hh
   *  \brief  logging
   **/
#ifndef LOGGING_HH_INCLUDED
#define LOGGING_HH_INCLUDED

#include <dune/common/exceptions.hh>
#include <dune/common/version.hh>
#include <dune/stuff/common/disable_warnings.hh>
#if DUNE_VERSION_NEWER(DUNE_COMMON, 2, 3)
#include <dune/common/parallel/mpihelper.hh>
#else
#include <dune/common/mpihelper.hh>
#endif
#include <dune/stuff/common/reenable_warnings.hh>

#include "misc.hh"
#include "filesystem.hh"
#include "logstreams.hh"
#include "string.hh"

#include <boost/range/adaptors.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include <assert.h>

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
              const std::string datadir = "data", const std::string _logdir = std::string("logs"));

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

} // namespace Common
} // namespace Stuff
} // namespace Dune

#define DSC_LOG Dune::Stuff::Common::Logger()
#define DSC_LOG_INFO DSC_LOG.info()
#define DSC_LOG_DEBUG DSC_LOG.debug()
#define DSC_LOG_ERROR DSC_LOG.error()

#define DSC_LOG_INFO_0 (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.info() : DSC_LOG.devnull())
#define DSC_LOG_DEBUG_0                                                                                                \
  (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.debug() : DSC_LOG.devnull())
#define DSC_LOG_ERROR_0                                                                                                \
  (Dune::MPIHelper::getCollectiveCommunication().rank() == 0 ? DSC_LOG.error() : DSC_LOG.devnull())

#endif // ifndef LOGGING_HH_INCLUDED

/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
