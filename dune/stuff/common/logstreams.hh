// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_LOGSTREAMS_HH
#define DUNE_STUFF_LOGSTREAMS_HH

#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>
#include <mutex>

#include <dune/common/timer.hh>

#include "memory.hh"
#include "string.hh"

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

  int pubsync();

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
}; // class SuspendableStrBuffer


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
}; // class FileBuffer


class EmptyBuffer : public SuspendableStrBuffer
{
public:
  EmptyBuffer(int loglevel, int& logflags)
    : SuspendableStrBuffer(loglevel, logflags)
  {
  }

protected:
  virtual int sync();
}; // class EmptyBuffer


class TimedPrefixedStreamBuffer : public std::basic_stringbuf<char, std::char_traits<char>>
{
  typedef std::basic_stringbuf<char, std::char_traits<char>> BaseType;

public:
  TimedPrefixedStreamBuffer(const Timer& timer, const std::string prefix, std::ostream& out = std::cout);

  virtual int sync();

private:
  TimedPrefixedStreamBuffer(const TimedPrefixedStreamBuffer&) = delete;

  std::string elapsed_time_str() const;

  const Timer& timer_;
  const std::string prefix_;
  std::ostream& out_;
  bool prefix_needed_;
  std::mutex mutex_;
}; // class TimedPrefixedStreamBuffer


class LogStream : StorageProvider<SuspendableStrBuffer>, public std::basic_ostream<char, std::char_traits<char>>
{
  typedef StorageProvider<SuspendableStrBuffer> StorageBaseType;
  typedef std::basic_ostream<char, std::char_traits<char>> BaseType;

public:
  typedef SuspendableStrBuffer::PriorityType PriorityType;
  static const PriorityType default_suspend_priority = SuspendableStrBuffer::default_suspend_priority;

  LogStream(SuspendableStrBuffer* buffer)
    : StorageBaseType(buffer)
    , BaseType(&this->storage_access())
  {
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
    assert(&this->storage_access());
    this->storage_access().suspend(priority);
  }

  /** \brief start accepting input into the buffer again
     * no-op if not suspended
     ***/
  void resume(PriorityType priority = default_suspend_priority)
  {
    assert(&this->storage_access());
    this->storage_access().resume(priority);
  } // Resume
}; // LogStream


class TimedPrefixedLogStream : StorageProvider<TimedPrefixedStreamBuffer>,
                               public std::basic_ostream<char, std::char_traits<char>>
{
  typedef StorageProvider<TimedPrefixedStreamBuffer> StorageBaseType;
  typedef std::basic_ostream<char, std::char_traits<char>> OstreamBaseType;

public:
  TimedPrefixedLogStream(const Timer& timer, const std::string prefix, std::ostream& out);

  virtual ~TimedPrefixedLogStream();
}; // TimedPrefixedLogStream


//! ostream compatible class wrapping file and console output
class FileLogStream : public LogStream
{
public:
  FileLogStream(int loglevel, int& logflags, /*std::ofstream& file,*/ std::ofstream& fileWoTime)
    : LogStream(new FileBuffer(loglevel, logflags, /*file,*/ fileWoTime))
  {
  }
}; // class FileLogStream


//! /dev/null
class EmptyLogStream : public LogStream
{
public:
  EmptyLogStream(int& logflags)
    : LogStream(new EmptyBuffer(int(LOG_NONE), logflags))
  {
  }
}; // class EmptyLogStream


namespace {
int dev_null_logflag;
EmptyLogStream dev_null(dev_null_logflag);
}


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // LOGSTREAMS_HH
