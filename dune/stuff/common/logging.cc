// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include <boost/format.hpp>

#include <dune/common/unused.hh>

#include "memory.hh"
#include "exceptions.hh"
#include "filesystem.hh"
#include "logging.hh"

namespace Dune {
namespace Stuff {
namespace Common {

Logging::Logging()
  : streamIDs_({LOG_ERROR, LOG_DEBUG, LOG_INFO})
  , logflags_(LOG_NONE)
  , emptyLogStream_(logflags_)
{
  for (const auto id : streamIDs_)
    streammap_[id] = make_unique<EmptyLogStream>(logflags_);
}

void Logging::deinit()
{
  streammap_.clear();
  if ((logflags_ & LOG_FILE) != 0) {
    logfile_ << std::endl;
    logfile_.close();
  }
}

Logging::~Logging()
{
  deinit();
}

void Logging::create(int logflags, const std::string logfile, const std::string datadir, const std::string _logdir)
{
  using namespace boost::filesystem;
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  boost::format log_fn("%s%s");
  if (comm.size() > 1) {
    const std::string rank = (boost::format("%08d") % comm.rank()).str();
    log_fn                 = boost::format("%s_p" + rank + "_%s");
  }
  logflags_   = logflags;
  path logdir = path(datadir) / _logdir;
  filename_ = logdir / (log_fn % logfile % ".log").str();
  testCreateDirectory(filename_.string());
  if ((logflags_ & LOG_FILE) != 0) {
    logfile_.open(filename_);
    assert(logfile_.is_open());
  }

  for (const auto id : streamIDs_) {
    flagmap_[id]   = logflags;
    streammap_[id] = make_unique<FileLogStream>(id, flagmap_[id], logfile_);
  }
} // create

void Logging::setPrefix(std::string prefix)
{
  deinit();
  create(logflags_, prefix);
} // setPrefix

void Logging::setStreamFlags(int streamID, int flags)
{
  assert(flagmap_.find(streamID) != flagmap_.end());
  // this might result in logging to diff targtes, so we flush the current targets
  flush();
  flagmap_[streamID] = flags;
}

int Logging::getStreamFlags(int streamID) const
{
  const auto it = flagmap_.find(streamID);
  if (it == flagmap_.end())
    DUNE_THROW(InvalidStateException,
               "cannot get flags for unkown Stream id, maybe you forgot to call \"Logging::create\"?");
  return it->second;
}

LogStream& Logging::getStream(int streamId)
{
  const auto it = streammap_.find(streamId);
  if (it == streammap_.end())
    DUNE_THROW(InvalidStateException, "cannot get unknown logStream, maybe you forgot to call \"Logging::create\"?");
  else {
    assert(it->second);
    return *(it->second);
  }
}

void Logging::flush()
{
  for (auto& pair : streammap_) {
    assert(pair.second);
    pair.second->flush();
  }
} // flush

int Logging::addStream(int flags)
{
  static int streamID_int = LOG_NEXT;
  streamID_int <<= 1;
  int streamID = streamID_int;
  streamIDs_.push_back(streamID);
  flagmap_[streamID]   = (flags | streamID);
  streammap_[streamID] = make_unique<FileLogStream>(streamID, flagmap_[streamID], logfile_);
  return streamID_int;
} // addStream

void Logging::resume(LogStream::PriorityType prio)
{
  for (auto& pair : streammap_) {
    pair.second->resume(prio);
  }
} // resume

void Logging::suspend(LogStream::PriorityType prio)
{
  for (auto& pair : streammap_) {
    pair.second->suspend(prio);
  }
} // suspend


TimedLogManager::TimedLogManager(const Timer& timer, const std::string info_prefix, const std::string debug_prefix,
                                 const std::string warning_prefix, const ssize_t max_info_level,
                                 const ssize_t max_debug_level, const bool enable_warnings,
                                 std::atomic<ssize_t>& current_level, std::ostream& disabled_out,
                                 std::ostream& enabled_out, std::ostream& warn_out)
  : timer_(timer)
  , current_level_(current_level)
  , info_(std::make_shared<TimedPrefixedLogStream>(timer_, info_prefix,
                                                   current_level_ <= max_info_level ? enabled_out : disabled_out))
  , debug_(std::make_shared<TimedPrefixedLogStream>(timer_, debug_prefix,
#ifdef NDEBUG
                                                    current_level_ <= max_debug_level ? enabled_out : dev_null))
#else
                                                    current_level_ <= max_debug_level ? enabled_out : disabled_out))
#endif
  , warn_(std::make_shared<TimedPrefixedLogStream>(timer_, warning_prefix, enable_warnings ? warn_out : disabled_out))
{
}

TimedLogManager::~TimedLogManager()
{
  --current_level_;
}

std::ostream& TimedLogManager::info()
{
  return *info_;
}

std::ostream& TimedLogManager::debug()
{
  return *debug_;
}

std::ostream& TimedLogManager::warn()
{
  return *warn_;
}


TimedLogging::TimedLogging()
  : max_info_level_(default_max_info_level)
  , max_debug_level_(default_max_debug_level)
  , enable_warnings_(default_enable_warnings)
  , enable_colors_(default_enable_colors && terminal_supports_color())
  , info_prefix_(enable_colors_ ? default_info_color() : "")
  , debug_prefix_(enable_colors_ ? default_debug_color() : "")
  , warning_prefix_(enable_colors_ ? default_warning_color() : "")
  , info_suffix_(enable_colors_ ? StreamModifiers::normal : "")
  , debug_suffix_(enable_colors_ ? StreamModifiers::normal : "")
  , warning_suffix_(enable_colors_ ? StreamModifiers::normal : "")
  , created_(false)
  , current_level_(-1)
{
  update_colors();
}

void TimedLogging::create(const ssize_t max_info_level, const ssize_t max_debug_level, const bool enable_warnings,
                          const bool enable_colors, const std::string info_color, const std::string debug_color,
                          const std::string warning_color)
{
  std::lock_guard<std::mutex> DUNE_UNUSED(guard)(mutex_);
  if (created_)
    DUNE_THROW(Exceptions::you_are_using_this_wrong, "Do not call create() more than once!");
  max_info_level_  = max_info_level;
  max_debug_level_ = max_debug_level;
  enable_warnings_ = enable_warnings;
  enable_colors_   = enable_colors && terminal_supports_color();
  info_prefix_     = enable_colors_ ? info_color : "";
  debug_prefix_    = enable_colors_ ? debug_color : "";
  warning_prefix_  = enable_colors_ ? warning_color : "";
  created_         = true;
  current_level_ = -1;
  update_colors();
} // ... create(...)

TimedLogManager TimedLogging::get(const std::string id)
{
  std::lock_guard<std::mutex> DUNE_UNUSED(guard)(mutex_);
  ++current_level_;
  return TimedLogManager(timer_,
                         info_prefix_ + (id.empty() ? "info" : id) + ": " + info_suffix_,
                         debug_prefix_ + (id.empty() ? "debug" : id) + ": " + debug_suffix_,
                         warning_prefix_ + (id.empty() ? "warn" : id) + ": " + warning_suffix_,
                         max_info_level_,
                         max_debug_level_,
                         enable_warnings_,
                         current_level_);
}

void TimedLogging::update_colors()
{
  if (enable_colors_) {
    info_prefix_    = color(info_prefix_);
    debug_prefix_   = color(debug_prefix_);
    warning_prefix_ = color(warning_prefix_);
    if (info_prefix_.empty())
      info_suffix_ = "";
    else {
      info_prefix_ += StreamModifiers::bold;
      info_suffix_ = StreamModifiers::normal;
    }
    if (debug_prefix_.empty())
      debug_suffix_ = "";
    else {
      debug_prefix_ += StreamModifiers::bold;
      debug_suffix_ = StreamModifiers::normal;
    }
    if (warning_prefix_.empty())
      warning_suffix_ = "";
    else {
      warning_prefix_ += StreamModifiers::bold;
      warning_suffix_ = StreamModifiers::normal;
    }
  }
} // ... update_colors(...)


TimedLogging& TimedLogger()
{
  static TimedLogging timed_logger;
  return timed_logger;
}


} // namespace Common
} // namespace Stuff
} // namespace Dune
