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

// ! only for logging to a single file which should then be executable by matlab
class MatlabLogStream : public std::basic_ostream<char, std::char_traits<char>>
{
  typedef std::basic_ostream<char, std::char_traits<char>> BaseType;

public:
  MatlabLogStream(LogFlags loglevel, int& logflags, std::ofstream& logFile)
    : BaseType(buffer_.rdbuf())
    , matlabLogFile_(logFile)
    , loglevel_(loglevel)
    , logflags_(logflags)
    , suspended_logflags_(logflags)
    , is_suspended_(false)
  {
  }

  ~MatlabLogStream()
  {
  }

  template <typename T>
  MatlabLogStream& operator<<(T in)
  {
    if (logflags_ & loglevel_)
      buffer_ << in;
    return *this;
  }

  MatlabLogStream& operator<<(MatlabLogStream& (*pf)(MatlabLogStream&))
  {
    if (logflags_ & loglevel_)
      buffer_ << pf;
    return *this;
  }

  MatlabLogStream& operator<<(std::ostream& (*pf)(std::ostream&))
  {
    if (logflags_ & loglevel_) {
      if (pf == (std::ostream & (*)(std::ostream&))std::endl) {
        // flush buffer into stream
        buffer_ << "\n";
        flush();
      } else {
        buffer_ << pf;
      }
    }
    return *this;
  } // <<

  void flush()
  {
    matlabLogFile_ << buffer_.str();
    matlabLogFile_.flush();
    buffer_.flush();
    buffer_.str(""); // clear the buffer
  }

  void suspend()
  {
    // don't accidentally invalidate flags if already suspended
    if (!is_suspended_) {
      suspended_logflags_ = logflags_;
      logflags_           = 1;
    }
    is_suspended_ = true;
  } // Suspend

  void resume()
  {
    if (is_suspended_)
      logflags_   = suspended_logflags_;
    is_suspended_ = false;
  }

  std::ofstream& fileStream()
  {
    return matlabLogFile_;
  }

private:
  std::stringstream buffer_;
  std::ofstream& matlabLogFile_;
  LogFlags loglevel_;
  int& logflags_;
  int suspended_logflags_;
  bool is_suspended_;
};

// ! ostream compatible class wrapping file and console output
class LogStream : public std::basic_ostream<char, std::char_traits<char>>
{
  typedef std::basic_ostream<char, std::char_traits<char>> BaseType;

public:
  typedef int PriorityType;
  static const PriorityType default_suspend_priority = 0;

protected:
  LogFlags loglevel_;
  int& logflags_;
  int suspended_logflags_;
  std::stringstream buffer_;
  std::ofstream& logfile_;
  std::ofstream& logfileWoTime_;
  bool is_suspended_;
  PriorityType suspend_priority_;

public:
  LogStream(LogFlags loglevel, int& logflags, std::ofstream& file, std::ofstream& fileWoTime)
    : BaseType(buffer_.rdbuf())
    , loglevel_(loglevel)
    , logflags_(logflags)
    , suspended_logflags_(logflags)
    , logfile_(file)
    , logfileWoTime_(fileWoTime)
    , is_suspended_(false)
    , suspend_priority_(default_suspend_priority)
  {
  }

  ~LogStream()
  {
    flush();
  }

  template <typename T>
  LogStream& operator<<(T in)
  {
    SetColor();
    if (logflags_ & loglevel_)
      buffer_ << in;
    UnsetColor();
    return *this;
  } // <<

  void suspend(PriorityType priority = default_suspend_priority)
  {
    // the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
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

  void resume(PriorityType priority = default_suspend_priority)
  {
    if (priority >= suspend_priority_) {
      if (is_suspended_)
        logflags_       = suspended_logflags_;
      is_suspended_     = false;
      suspend_priority_ = default_suspend_priority;
    }
  } // Resume


  void flush()
  {
    if (logflags_ & loglevel_) {
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

  void SetColor()
  {
    // if ( logflags_ & LOG_INFO ) {
    // buffer_ << "\033[21;31m";
    // }
  }

  void UnsetColor()
  {
    // buffer_ << "\033[0m";
  }

  // template < class Class >
  LogStream& operator<<(LogStream& (*pf)(LogStream&))
  {
    if (logflags_ & loglevel_)
      buffer_ << pf;
    return *this;
  }

  LogStream& operator<<(std::ostream& (*pf)(std::ostream&))
  {
    SetColor();
    if (logflags_ & loglevel_) {
      if (pf == (std::ostream & (*)(std::ostream&))std::endl) {
        // flush buffer into stream
        buffer_ << "\n";
        flush();
      } else
        buffer_ << pf;
    }
    UnsetColor();
    return *this;
  } // <<

  template <class Class, typename Pointer>
  void Log(Pointer pf, Class& c)
  {
    if (logflags_ & loglevel_) {
      (c.*pf)(buffer_);
    }
  }
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
