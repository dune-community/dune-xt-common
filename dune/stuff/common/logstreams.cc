// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "logstreams.hh"

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

LogStream& LogStream::flush()
{
  assert(this->buffer_);
  this->buffer_->pubsync();
  return *this;
}


int FileLogStream::FileBuffer::sync()
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


int EmptyLogStream::EmptyBuffer::sync()
{
  str("");
  return 0;
}

} // namespace Common
} // namespace Stuff
} // namespace Dune
