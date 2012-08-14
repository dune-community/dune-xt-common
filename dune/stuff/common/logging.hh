/**
   *  \file logging.hh
   *  \brief  logging
   **/
#ifndef LOGGING_HH_INCLUDED
#define LOGGING_HH_INCLUDED

#include <fstream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include <assert.h>

// dune-common
#include <dune/common/exceptions.hh>

#include "misc.hh"
#include "filesystem.hh"
#include "logstreams.hh"
#include "string.hh"

#include <boost/range/adaptors.hpp>
#include <boost/foreach.hpp>

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
  Logging()
    : logflags_(LOG_NONE)
    , matlabLogStreamPtr(0)
    , emptyLogStream_(logflags_)
  {
    streamIDs_.push_back(LOG_ERROR);
    streamIDs_.push_back(LOG_DEBUG);
    streamIDs_.push_back(LOG_INFO);
  }

public:
  ~Logging()
  {
    // destroy in reverse creation order, 2012 style
    BOOST_REVERSE_FOREACH(auto id, streamIDs_)
    {
      // logstream dtor is mandated to flush itself
      delete streammap_[id];
      streammap_[id] = 0;
    }

    if ((logflags_ & LOG_FILE) != 0) {
      logfile_ << '\n' << Dune::Stuff::Common::String::fromTime() << ": LOG END" << std::endl;
      logfileWoTime_ << std::endl;
      logfile_.close();
      logfileWoTime_.close();
    }

    // delete the MatlabLogStream
    if (matlabLogStreamPtr) {
      matlabLogStreamPtr->flush();
      matlabLogFile_.close();
      delete matlabLogStreamPtr;
      matlabLogStreamPtr = 0;
    }
  }

  /** \brief setup loglevel, logfilename
     *  \param logflags any OR'd combination of flags
     *  \param logfile filename for log, can contain paths, but creation will fail if dir is non-existant
     **/
  void create(int logflags = (LOG_FILE | LOG_CONSOLE | LOG_ERROR), std::string logfile = "dune_stuff_log",
              std::string datadir = "data", std::string logdir = std::string(""))
  {
    logflags_ = logflags;

    // if we get a logdir from parameters append path seperator, otherwise leave empty
    // enables us to use logdir unconditionally further down
    if (!datadir.empty())
      datadir += "/";
    if (!logdir.empty())
      logdir += "/";
    logdir = datadir + logdir;

    filename_ = logdir + logfile + "_time.log";
    Stuff::Common::Filesystem::testCreateDirectory(logdir);

    filenameWoTime_ = logdir + logfile + ".log";
    if ((logflags_ & LOG_FILE) != 0) {
      logfile_.open(filename_.c_str());
      assert(logfile_.is_open());
      logfileWoTime_.open(filenameWoTime_.c_str());
      assert(logfileWoTime_.is_open());
    }
    IdVecCIter it = streamIDs_.begin();
    for (; it != streamIDs_.end(); ++it) {
      flagmap_[*it]   = logflags;
      streammap_[*it] = new FileLogStream(*it, flagmap_[*it], logfile_, logfileWoTime_);
    }
    // create the MatlabLogStream
    std::string matlabLogFileName = logdir + logfile + "_matlab.m";
    Stuff::Common::Filesystem::testCreateDirectory(matlabLogFileName); // could assert this if i figure out
    // why errno is != EEXIST
    matlabLogFile_.open(matlabLogFileName.c_str());
    assert(matlabLogFile_.is_open());
    matlabLogStreamPtr = new MatlabLogStream(LOG_FILE, logflags_, matlabLogFile_);
  } // Create

  // ! \attention This will probably not do wht we want it to!
  void setPrefix(std::string prefix)
  {
    // / begin dtor
    IdVecCIter it = streamIDs_.end();

    for (; it != streamIDs_.begin(); --it) {
      delete streammap_[*it];
      streammap_[*it] = 0;
    }

    if ((logflags_ & LOG_FILE) != 0) {
      logfile_ << '\n' << Dune::Stuff::Common::String::fromTime() << ": LOG END" << std::endl;
      logfileWoTime_ << std::endl;
      logfile_.close();
      logfileWoTime_.close();
    }

    // delete the MatlabLogStream
    matlabLogStreamPtr->flush();
    matlabLogFile_.close();
    delete matlabLogStreamPtr;
    matlabLogStreamPtr = 0;
    // / end dtor

    create(logflags_, prefix);
  } // SetPrefix

  void setStreamFlags(int streamID, int flags)
  {
    assert(flagmap_.find(streamID) != flagmap_.end());
    // this might result in logging to diff targtes, so we flush the current targets
    flush();
    flagmap_[streamID] = flags;
  }

  int getStreamFlags(int streamID) const
  {
    const auto it = flagmap_.find(streamID);
    if (it == flagmap_.end())
      DUNE_THROW(InvalidStateException, "cannot get flags for unkown Stream id");
    return it->second;
  }

  /** \name forwarded Log functions
     * \{
     */
  template <typename Pointer, class Class>
  void log(void (Class::*pf)(std::ostream&) const, Class& c, int streamID)
  {
    getStream(streamID).log(pf, c);
  } // Log

  template <class Class, typename Pointer>
  void log(Pointer pf, Class& c, int streamID)
  {
    getStream(streamID).log(pf, c);
  } // Log

  template <class T>
  void log(T c, int streamID)
  {
    getStream(streamID) << c;
  } // Log

  /** \}
     */

  LogStream& getStream(int streamId)
  {
    const auto it = streammap_.find(streamId);
    if (it == streammap_.end())
      return emptyLogStream_;
    //      DUNE_THROW(InvalidStateException, "cannot get unkown Stream");
    else
      return *(it->second);
  }

  LogStream& err() DUNE_DEPRECATED_MSG("use error() instead")
  {
    return getStream(LOG_ERROR);
  }
  LogStream& error()
  {
    return getStream(LOG_ERROR);
  }
  LogStream& info()
  {
    return getStream(LOG_INFO);
  }
  LogStream& dbg() DUNE_DEPRECATED_MSG("use debug() instead")
  {
    return getStream(LOG_DEBUG);
  }
  LogStream& debug()
  {
    return getStream(LOG_DEBUG);
  }
  LogStream& devnull()
  {
    return emptyLogStream_;
  }
  LogStream& matlab()
  {
    if (matlabLogStreamPtr)
      return *matlabLogStreamPtr;
    else
      return emptyLogStream_;
  }

  void flush()
  {
    BOOST_FOREACH (auto pair, streammap_) {
      pair.second->flush();
    }
  } // Flush

  int addStream(int flags)
  {
    static int streamID_int = LOG_NEXT;
    streamID_int <<= 1;
    int streamID = streamID_int;
    streamIDs_.push_back(streamID);
    flagmap_[streamID]   = (flags | streamID);
    streammap_[streamID] = new FileLogStream(streamID, flagmap_[streamID], logfile_, logfileWoTime_);
    return streamID_int;
  } // AddStream

  void resume(LogStream::PriorityType prio = LogStream::default_suspend_priority)
  {
    BOOST_FOREACH (auto pair, streammap_) {
      pair.second->resume(prio);
    }
  } // Resume

  void suspend(LogStream::PriorityType prio = LogStream::default_suspend_priority)
  {
    BOOST_FOREACH (auto pair, streammap_) {
      pair.second->suspend(prio);
    }
  } // Suspend

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
  std::string filename_;
  std::string filenameWoTime_;
  std::ofstream logfile_;
  std::ofstream logfileWoTime_;
  std::ofstream matlabLogFile_;
  typedef std::map<int, int> FlagMap;
  FlagMap flagmap_;
  typedef std::map<int, LogStream*> StreamMap;
  StreamMap streammap_;
  typedef std::vector<int> IdVec;
  typedef std::vector<int>::const_iterator IdVecCIter;
  IdVec streamIDs_;
  int logflags_;
  MatlabLogStream* matlabLogStreamPtr;
  EmptyLogStream emptyLogStream_;

  friend Logging& Logger();
  // satisfy stricter warnings wrt copying
  Logging(const Logging&);
  Logging& operator=(const Logging&);
};

// !global Logging instance
Logging& Logger()
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
