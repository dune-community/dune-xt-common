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
  LOG_ERROR   = 2,
  LOG_INFO    = 4,
  LOG_DEBUG   = 8,
  LOG_CONSOLE = 16,
  LOG_FILE    = 32,
  LOG_NEXT    = 64
};

class SuspendableStrBuffer : public std::basic_stringbuf<char, std::char_traits<char>>
{
  typedef std::basic_stringbuf<char, std::char_traits<char>> BaseType;

public:
  typedef int PriorityType;
  static const PriorityType default_suspend_priority = 0;

  SuspendableStrBuffer(int loglevel, int& logflags)
    : logflags_(logflags)
    , loglevel_(loglevel)
    , suspended_logflags_(logflags)
    , is_suspended_(false)
    , suspend_priority_(default_suspend_priority)
  {
  }

  /** \brief stop accepting input into the buffer
     * the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
     * no-op if already suspended
     ***/
  void suspend(PriorityType priority)
  {
    suspend_priority_ = std::max(priority, suspend_priority_);
    {
      // don't accidentally invalidate flags if already suspended
      if (!is_suspended_) {
        suspended_logflags_ = logflags_;
        logflags_           = LOG_NONE;
      }
      is_suspended_ = true;
    }
  } // Suspend

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority)
  {
    if (priority >= suspend_priority_) {
      if (is_suspended_)
        logflags_       = suspended_logflags_;
      is_suspended_     = false;
      suspend_priority_ = default_suspend_priority;
    }
  } // Resume

protected:
  virtual std::streamsize xsputn(const char_type* s, std::streamsize count)
  {
    if (enabled())
      return BaseType::xsputn(s, count);
    // pretend everything was written
    return std::streamsize(count);
  }

  virtual int_type overflow(int_type ch = traits_type::eof())
  {
    if (enabled())
      return BaseType::overflow(ch);
    // anything not equal to traits::eof is considered a success
    return traits_type::eof() + 1;
  }

private:
  inline bool enabled() const
  {
    return (!is_suspended_) && (logflags_ & loglevel_);
  }

  int& logflags_;
  int loglevel_;
  int suspended_logflags_;
  bool is_suspended_;
  PriorityType suspend_priority_;
};


class LogStream : public std::ostream
{
public:
  typedef SuspendableStrBuffer::PriorityType PriorityType;
  static const PriorityType default_suspend_priority = SuspendableStrBuffer::default_suspend_priority;

  LogStream(int loglevel, int& logflags)
    : buffer_(loglevel, logflags)
    , std::ostream(&buffer_)
  {
  }

  virtual ~LogStream()
  {
  }

  /** \brief forwards suspend to buffer
     * the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
     * no-op if already suspended
     ***/
  void suspend(PriorityType priority = default_suspend_priority)
  {
    buffer_.suspend(priority);
  } // Suspend

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority = default_suspend_priority)
  {
    buffer_.resume(priority);
  } // Resume

  template <class Class, typename Pointer>
  void log(Pointer pf, Class& c)
  {
    (c.*pf)(buffer_);
  }

  //! dump buffer into file/stream and clear it
  virtual void flush() = 0;

protected:
  SuspendableStrBuffer buffer_;

}; // LogStream

//! only for logging to a single file which should then be executable by matlab
class MatlabLogStream : public LogStream
{
public:
  MatlabLogStream(int loglevel, int& logflags, std::ofstream& logFile)
    : LogStream(loglevel, logflags)
    , matlabLogFile_(logFile)
  {
  }

  virtual ~MatlabLogStream()
  {
    flush();
  }

  //! dump buffer into file/stream and clear it
  void flush()
  {
    matlabLogFile_ << buffer_.str();
    matlabLogFile_.flush();
    buffer_.str("");
  }

  //! there should be no need to at the fstream directly
  const std::ofstream& fileStream() const DUNE_DEPRECATED
  {
    return matlabLogFile_;
  }

private:
  std::ofstream& matlabLogFile_;
}; // class MatlabLogStream

//! ostream compatible class wrapping file and console output
class FileLogStream : public LogStream
{
public:
  FileLogStream(int loglevel, int& logflags, std::ofstream& file, std::ofstream& fileWoTime)
    : LogStream(loglevel, logflags)
    , logfile_(file)
    , logfileWoTime_(fileWoTime)
  {
  }

  virtual ~FileLogStream()
  {
    flush();
  }

  void flush()
  {
    // flush buffer into stream
    std::cout << buffer_.str();
    std::cout.flush();
    logfile_ << "\n" << Dune::Stuff::Common::String::fromTime() << buffer_.str() << std::endl;
    logfileWoTime_ << buffer_.str();
    logfile_.flush();
    logfileWoTime_.flush();
    buffer_.str("");
  } // Flush

private:
  std::ofstream& logfile_;
  std::ofstream& logfileWoTime_;
}; // class FileLogStream

// /dev/null
class EmptyLogStream : public LogStream
{
public:
  EmptyLogStream(int& logflags)
    : LogStream(int(LOG_NONE), logflags)
  {
  }

  void flush()
  {
    buffer_.str("");
  }
}; // class EmptyLogStream

namespace {
int dev_null_logflag;
EmptyLogStream dev_null_(dev_null_logflag);
}

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
