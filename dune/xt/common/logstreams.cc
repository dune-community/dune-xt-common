// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014)
//   Rene Milk       (2013 - 2015)
//   Sven Kaulmann   (2014)

#include "config.h"
#include "logstreams.hh"

#include <dune/common/unused.hh>

namespace Dune {
namespace Stuff {
namespace Common {

SuspendableStrBuffer::SuspendableStrBuffer(int loglevel, int& logflags)
  : logflags_(logflags)
  , loglevel_(loglevel)
  , suspended_logflags_(logflags)
  , is_suspended_(false)
  , suspend_priority_(default_suspend_priority)
{
}

void SuspendableStrBuffer::suspend(PriorityType priority)
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

void SuspendableStrBuffer::resume(PriorityType priority)
{
  if (priority >= suspend_priority_) {
    if (is_suspended_)
      logflags_       = suspended_logflags_;
    is_suspended_     = false;
    suspend_priority_ = default_suspend_priority;
  }
} // Resume

std::streamsize SuspendableStrBuffer::xsputn(const char_type* s, std::streamsize count)
{
  if (enabled()) {
    std::lock_guard<std::mutex> guard(mutex_);
    return BaseType::xsputn(s, count);
  }
  // pretend everything was written
  return std::streamsize(count);
}

SuspendableStrBuffer::int_type SuspendableStrBuffer::overflow(SuspendableStrBuffer::int_type ch)
{
  if (enabled())
    return BaseType::overflow(ch);
  // anything not equal to traits::eof is considered a success
  return traits_type::eof() + 1;
}

int SuspendableStrBuffer::pubsync()
{
  if (enabled())
    return BaseType::pubsync();
  return 0;
}

TimedPrefixedStreamBuffer::TimedPrefixedStreamBuffer(const Timer& timer, const std::string prefix, std::ostream& out)
  : timer_(timer)
  , prefix_(prefix)
  , out_(out)
  , prefix_needed_(true)
{
}

int TimedPrefixedStreamBuffer::sync()
{
  std::lock_guard<std::mutex> DUNE_UNUSED(guard)(mutex_);
  const std::string tmp_str = str();
  if (prefix_needed_ && !tmp_str.empty()) {
    out_ << elapsed_time_str() << prefix_;
    prefix_needed_ = false;
  }
  auto lines = tokenize(tmp_str, "\n", boost::algorithm::token_compress_off);
  assert(lines.size() > 0);
  out_ << lines[0];
  for (size_t ii = 1; ii < lines.size() - 1; ++ii)
    out_ << "\n" << elapsed_time_str() << prefix_ << lines[ii];
  if (lines.size() > 1) {
    out_ << "\n";
    const auto& last = lines.back();
    if (last.empty())
      prefix_needed_ = true;
    else
      out_ << elapsed_time_str() << prefix_ << last;
  }
  out_.flush();
  str("");
  return 0;
} // ... sync(...)

std::string TimedPrefixedStreamBuffer::elapsed_time_str() const
{
  const double secs_per_week = 604800;
  const double secs_per_day  = 86400;
  const double secs_per_hour = 3600;
  const double elapsed = timer_.elapsed();
  const size_t weeks(elapsed / secs_per_week);
  const size_t days((elapsed - weeks * secs_per_week) / secs_per_day);
  const size_t hours((elapsed - weeks * secs_per_week - days * secs_per_day) / 3600.0);
  const size_t minutes((elapsed - weeks * secs_per_week - days * secs_per_day - hours * secs_per_hour) / 60.0);
  const size_t seconds(elapsed - weeks * secs_per_week - days * secs_per_day - hours * secs_per_hour - minutes * 60);
  if (elapsed > secs_per_week) // more than a week
    return (boost::format("%02dw %02dd %02d:%02d:%02d|") % weeks % days % hours % minutes % seconds).str();
  else if (elapsed > secs_per_day) // less than a week, more than a day
    return (boost::format("%02dd %02d:%02d:%02d|") % days % hours % minutes % seconds).str();
  else if (elapsed > secs_per_hour) // less than a day, more than one hour
    return (boost::format("%02d:%02d:%02d|") % hours % minutes % seconds).str();
  else // less than one hour
    return (boost::format("%02d:%02d|") % minutes % seconds).str();
} // ... elapsed_time(...)

LogStream& LogStream::flush()
{
  assert(&this->storage_access());
  this->storage_access().pubsync();
  return *this;
}

TimedPrefixedLogStream::TimedPrefixedLogStream(const Timer& timer, const std::string prefix, std::ostream& outstream)
  : StorageBaseType(new TimedPrefixedStreamBuffer(timer, prefix, outstream))
  , OstreamBaseType(&this->storage_access())
{
}

TimedPrefixedLogStream::~TimedPrefixedLogStream()
{
  flush();
}

int FileBuffer::sync()
{
  // flush buffer into stream
  std::lock_guard<std::mutex> guard(sync_mutex_);
  std::cout << str();
  std::cout.flush();
  logfile_ << str();
  logfile_.flush();
  str("");
  return 0;
}

int EmptyBuffer::sync()
{
  str("");
  return 0;
}

} // namespace Common
} // namespace Stuff
} // namespace Dune
