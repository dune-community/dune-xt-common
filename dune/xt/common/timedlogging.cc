// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016 - 2017)
//   Rene Milk       (2012 - 2016, 2018)
//   Tobias Leibner  (2016)

#include "config.h"
#include "timedlogging.hh"

#include <boost/format.hpp>

#include <dune/common/unused.hh>

#include "memory.hh"
#include "exceptions.hh"
#include "filesystem.hh"

namespace Dune {
namespace XT {
namespace Common {

TimedLogManager::TimedLogManager(const Timer& timer,
                                 const std::string info_prefix,
                                 const std::string debug_prefix,
                                 const std::string warning_prefix,
                                 const ssize_t max_info_level,
                                 const ssize_t max_debug_level,
                                 const bool enable_warnings,
                                 std::atomic<ssize_t>& current_level,
                                 std::ostream& disabled_out,
                                 std::ostream& enabled_out,
                                 std::ostream& warn_out)
  : timer_(timer)
  , current_level_(current_level)
  , info_(std::make_shared<TimedPrefixedLogStream>(
        timer_, info_prefix, current_level_ <= max_info_level ? enabled_out : disabled_out))
  , debug_(std::make_shared<TimedPrefixedLogStream>(timer_,
                                                    debug_prefix,
#if DUNE_XT_COMMON_TIMEDLOGGING_ENABLE_DEBUG
                                                    current_level_ <= max_debug_level ? enabled_out : disabled_out))
#else
                                                    current_level_ <= max_debug_level ? enabled_out : dev_null))
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

void TimedLogging::create(const ssize_t max_info_level,
                          const ssize_t max_debug_level,
                          const bool enable_warnings,
                          const bool enable_colors,
                          const std::string info_color,
                          const std::string debug_color,
                          const std::string warning_color)
{
  DUNE_UNUSED std::lock_guard<std::mutex> guard(mutex_);
  if (created_)
    DUNE_THROW(Exceptions::you_are_using_this_wrong, "Do not call create() more than once!");
  max_info_level_ = max_info_level;
  max_debug_level_ = max_debug_level;
  enable_warnings_ = enable_warnings;
  enable_colors_ = enable_colors && terminal_supports_color();
  info_prefix_ = enable_colors_ ? info_color : "";
  debug_prefix_ = enable_colors_ ? debug_color : "";
  warning_prefix_ = enable_colors_ ? warning_color : "";
  created_ = true;
  current_level_ = -1;
  update_colors();
} // ... create(...)

TimedLogManager TimedLogging::get(const std::string id)
{
  DUNE_UNUSED std::lock_guard<std::mutex> guard(mutex_);
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
    info_prefix_ = color(info_prefix_);
    debug_prefix_ = color(debug_prefix_);
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

DUNE_EXPORT TimedLogging& TimedLogger()
{
  static TimedLogging timed_logger;
  return timed_logger;
}

} // namespace Common
} // namespace XT
} // namespace Dune
