// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_LOGSTREAMS_HH
#define DUNE_STUFF_LOGSTREAMS_HH

#include "string.hh"
#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>
#include <mutex>

#include <dune/stuff/common/disable_warnings.hh>

namespace Dune {
namespace Stuff {
namespace Common {

enum LogFlags
{
  LOG_NONE    = 1,
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

  SuspendableStrBuffer(int loglevel, int& logflags);

  /** \brief stop accepting input into the buffer
     * the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
     * no-op if already suspended
     ***/
  void suspend(PriorityType priority);

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority);

protected:
  virtual std::streamsize xsputn(const char_type* s, std::streamsize count);
  virtual int_type overflow(int_type ch = traits_type::eof());

private:
  inline bool enabled() const
  {
    return (!is_suspended_) && (logflags_ & loglevel_);
  }

  SuspendableStrBuffer(const SuspendableStrBuffer&) = delete;

  int& logflags_;
  int loglevel_;
  int suspended_logflags_;
  bool is_suspended_;
  PriorityType suspend_priority_;
  std::mutex mutex_;
};


class LogStream : public std::basic_ostream<char, std::char_traits<char>>
{
  typedef std::basic_ostream<char, std::char_traits<char>> BaseType;

public:
  typedef SuspendableStrBuffer::PriorityType PriorityType;
  static const PriorityType default_suspend_priority = SuspendableStrBuffer::default_suspend_priority;

  LogStream(SuspendableStrBuffer* buffer)
    : BaseType(buffer_)
    , buffer_(buffer)
  {
  }

  /** \brief Tunnel buffer setting to ostream base
   * for some weird reason all derived classes MUST call
   * it again after base class init
   **/
  void rdbuf(SuspendableStrBuffer* buf)
  {
    buffer_ = buf;
    BaseType::rdbuf(buf);
  }

  virtual ~LogStream()
  {
    flush();
  }

  //! dump buffer into file/stream and clear it
  virtual LogStream& flush();

  /** \brief forwards suspend to buffer
     * the suspend_priority_ mechanism provides a way to silence streams from 'higher' modules
     * no-op if already suspended
     ***/
  void suspend(PriorityType priority = default_suspend_priority)
  {
    assert(buffer_);
    buffer_->suspend(priority);
  }

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority = default_suspend_priority)
  {
    assert(buffer_);
    buffer_->resume(priority);
  } // Resume

private:
  SuspendableStrBuffer* buffer_;
}; // LogStream


//! ostream compatible class wrapping file and console output
class FileLogStream : public LogStream
{
private:
  class FileBuffer : public SuspendableStrBuffer
  {
  public:
    FileBuffer(int loglevel, int& logflags, std::ofstream& file)
      : SuspendableStrBuffer(loglevel, logflags)
      , logfile_(file)
    {
    }

  private:
    std::ofstream& logfile_;
    std::mutex sync_mutex_;

  protected:
    virtual int sync();
  };

public:
  FileLogStream(int loglevel, int& logflags, /*std::ofstream& file,*/ std::ofstream& fileWoTime)
    : logBuffer_(loglevel, logflags, /*file,*/ fileWoTime)
    , LogStream(&logBuffer_)
  {
    LogStream::rdbuf(&logBuffer_);
  }

private:
  FileBuffer logBuffer_;
}; // class FileLogStream


//! /dev/null
class EmptyLogStream : public LogStream
{
private:
  class EmptyBuffer : public SuspendableStrBuffer
  {
  public:
    EmptyBuffer(int loglevel, int& logflags)
      : SuspendableStrBuffer(loglevel, logflags)
    {
    }

  protected:
    virtual int sync();
  };

public:
  EmptyLogStream(int& logflags)
    : logBuffer_(int(LOG_NONE), logflags)
    , LogStream(&logBuffer_)
  {
    LogStream::rdbuf(&logBuffer_);
  }

private:
  EmptyBuffer logBuffer_;
}; // class EmptyLogStream


namespace {
int dev_null_logflag;
EmptyLogStream dev_null(dev_null_logflag);
}

} // namespace Common
} // namespace Stuff
} // namespace Dune

#include <dune/stuff/common/reenable_warnings.hh>

#endif // LOGSTREAMS_HH
