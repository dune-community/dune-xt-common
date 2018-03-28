// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)
//   Tobias Leibner  (2017)

#ifndef DUNE_XT_COMMON_BINDINGS_HH
#define DUNE_XT_COMMON_BINDINGS_HH

#include <python/dune/xt/common/exceptions.bindings.hh>
#include <python/dune/xt/common/fvector.hh>
#include <python/dune/xt/common/fmatrix.hh>
#include <python/dune/xt/common/configuration.hh>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/string.hh>
#include <dune/xt/common/timedlogging.hh>

#include <dune/common/parallel/mpihelper.hh>

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>

#include <python/dune/xt/common/bindings.hh>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/timedlogging.hh>

namespace Dune {
namespace XT {
namespace Common {
namespace bindings {

static void add_initialization(pybind11::module& m, std::string logger_name)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

  m.def("_init_mpi",
        [](const std::vector<std::string>& args) {
          int argc = Dune::XT::Common::numeric_cast<int>(args.size());
          char** argv = Dune::XT::Common::vector_to_main_args(args);
          Dune::MPIHelper::instance(argc, argv);
        },
        "args"_a = std::vector<std::string>());

  m.def("_init_logger",
        [](const ssize_t max_info_level,
           const ssize_t max_debug_level,
           const bool enable_warnings,
           const bool enable_colors,
           const std::string& info_color,
           const std::string& debug_color,
           const std::string& warning_color) {
          Dune::XT::Common::TimedLogger().create(
              max_info_level, max_debug_level, enable_warnings, enable_colors, info_color, debug_color, warning_color);
        },
        "max_info_level"_a = std::numeric_limits<ssize_t>::max(),
        "max_debug_level"_a = std::numeric_limits<ssize_t>::max(),
        "enable_warnings"_a = true,
        "enable_colors"_a = true,
        "info_color"_a = "blue",
        "debug_color"_a = "darkgray",
        "warning_color"_a = "red");

  m.def("_test_logger",
        [=](const bool info, const bool debug, const bool warning) {
          auto logger = Dune::XT::Common::TimedLogger().get(logger_name);
          if (info)
            logger.info() << "info logging works!" << std::endl;
          if (debug)
            logger.debug() << "debug logging works!" << std::endl;
          if (warning)
            logger.warn() << "warning logging works!" << std::endl;
        },
        "info"_a = true,
        "debug"_a = true,
        "warning"_a = true);
}

} // namespace bindings
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_BINDINGS_HH
