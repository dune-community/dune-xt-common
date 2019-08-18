// This file is part of the dune-xt project:
//   https://github.com/dune-community/dune-xt
// Copyright 2009-2018 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2019)

#include "config.h"

#include <python/pybindxi/pybind11.h>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/timedlogging.hh>


PYBIND11_MODULE(timedlogging, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

  m.def(
      "init_logger",
      [](const ssize_t max_info_level,
         const ssize_t max_debug_level,
         const bool enable_warnings,
         const bool enable_colors,
         const std::string& info_color,
         const std::string& debug_color,
         const std::string& warning_color) {
        try {
          Dune::XT::Common::TimedLogger().create(
              max_info_level, max_debug_level, enable_warnings, enable_colors, info_color, debug_color, warning_color);
        } catch (Dune::XT::Common::Exceptions::logger_error&) {
        }
      },
      "max_info_level"_a = std::numeric_limits<ssize_t>::max(),
      "max_debug_level"_a = std::numeric_limits<ssize_t>::max(),
      "enable_warnings"_a = true,
      "enable_colors"_a = true,
      "info_color"_a = "blue",
      "debug_color"_a = "darkgray",
      "warning_color"_a = "red");
}
