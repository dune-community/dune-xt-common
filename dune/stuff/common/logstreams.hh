#ifndef DUNE_STUFF_LOGSTREAMS_HH
#define DUNE_STUFF_LOGSTREAMS_HH

#include "string.hh"
#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Dune {
namespace Stuff {
namespace Common {
enum LogFlags
{
  LOG_NONE    = 1,
  LOG_ERR     = 2,
  LOG_INFO    = 4,
  LOG_DEBUG   = 8,
  LOG_CONSOLE = 16,
  LOG_FILE    = 32
};

class ILogStream : public std::ostream
{
public:
  typedef int PriorityType;
  static const PriorityType default_suspend_priority = 0;

  ILogStream(LogFlags loglevel, int& logflags)
    : std::ostream(buffer_.rdbuf())
    , logflags_(logflags)
    , loglevel_(loglevel)
    , suspended_logflags_(logflags)
    , is_suspended_(false)
    , suspend_priority_(default_suspend_priority)
  {
  }

  virtual ~ILogStream()
  {
  }

  inline bool enabled() const
  {
    return (!is_suspended_) && (logflags_ & loglevel_);
  }

  template <typename T>
  ILogStream& operator<<(T in)
  {
    if (enabled())
      buffer_ << in;
    return *this;
  } // <<

  ILogStream& operator<<(std::ostream& (*pf)(std::ostream&))
  {
    if (enabled()) {
      buffer_ << pf;
      if (pf == (std::ostream & (*)(std::ostream&))std::endl) {
        flush();
      }
    }
    return *this;
  }

  template <class Class, typename Pointer>
  void log(Pointer pf, Class& c)
  {
    if (enabled()) {
      (c.*pf)(buffer_);
    }
  }

  /** \brief stop accepting input into the buffer
     * the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
     * no-op if already suspended
     ***/
  void suspend(PriorityType priority = default_suspend_priority)
  {
    suspend_priority_ = std::max(priority, suspend_priority_);
    {
      // don't accidentally invalidate flags if already suspended
      if (!is_suspended_) {
        suspended_logflags_ = logflags_;
        logflags_           = 1;
      }
      is_suspended_ = true;
    }
  } // Suspend

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority = default_suspend_priority)
  {
    if (priority >= suspend_priority_) {
      if (is_suspended_)
        logflags_       = suspended_logflags_;
      is_suspended_     = false;
      suspend_priority_ = default_suspend_priority;
    }
  } // Resume

  // ! dump buffer into file/stream and clear it
  virtual void flush() = 0;

protected:
  std::stringstream buffer_;
  int& logflags_;

private:
  LogFlags loglevel_;
  int suspended_logflags_;
  bool is_suspended_;
  PriorityType suspend_priority_;
};
// ! only for logging to a single file which should then be executable by matlab
class MatlabLogStream : public ILogStream
{
public:
  MatlabLogStream(LogFlags loglevel, int& logflags, std::ofstream& logFile)
    : ILogStream(loglevel, logflags)
    , matlabLogFile_(logFile)
  {
  }

  virtual ~MatlabLogStream()
  {
    flush();
  }

  // ! dump buffer into file/stream and clear it
  void flush()
  {
    matlabLogFile_ << buffer_.str();
    matlabLogFile_.flush();
    buffer_.flush();
    buffer_.clear();
  }

  // ! there should be no need to at the fstream directly
  const std::ofstream& fileStream() const DUNE_DEPRECATED
  {
    return matlabLogFile_;
  }

private:
  std::ofstream& matlabLogFile_;
};

// ! ostream compatible class wrapping file and console output
class LogStream : public ILogStream
{
public:
  LogStream(LogFlags loglevel, int& logflags, std::ofstream& file, std::ofstream& fileWoTime)
    : ILogStream(loglevel, logflags)
    , logfile_(file)
    , logfileWoTime_(fileWoTime)
  {
  }

  virtual ~LogStream()
  {
    flush();
  }

  void flush()
  {
    if (enabled()) {
      // flush buffer into stream
      if ((logflags_ & LOG_CONSOLE) != 0) {
        std::cout << buffer_.str(); // << std::endl;
        std::cout.flush();
      }
      if ((logflags_ & LOG_FILE) != 0) {
        logfile_ << "\n" << Dune::Stuff::Common::String::fromTime() << buffer_.str() << std::endl;
        logfileWoTime_ << buffer_.str(); // << std::endl;
        logfile_.flush();
        logfileWoTime_.flush();
      }
      buffer_.flush();
      buffer_.str(""); // clear the buffer
    }
  } // Flush

private:
  std::ofstream& logfile_;
  std::ofstream& logfileWoTime_;
};
} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // LOGSTREAMS_HH
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
