// This file is part of the dune-xt project:
//   https://github.com/dune-community/dune-xt
// Copyright 2009-2018 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   René Fritze (2018)

#include "config.h"

#include <string>
#include <vector>

#include <dune/common/parallel/mpihelper.hh>
#include <python/pybindxi/pybind11.h>
#include <python/pybindxi/stl.h>

#include <python/dune/xt/common/bindings.hh>
#include <dune/xt/common/python.hh>
#include <dune/xt/common/logging.hh>
#include <python/dune/xt/common/exceptions.bindings.hh>


PYBIND11_MODULE(logging, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;
  using namespace Dune::XT::Common;

  m.def("create",
        [](int logflags, const std::string logfile, const std::string datadir, const std::string _logdir) {
          Logger().create(logflags, logfile, datadir, _logdir);
        },
        "logflags"_a = LogDefault,
        "logfile"_a = "dune_xt_common_log",
        "datadir"_a = "data",
        "_logdir"_a = std::string("log"));
  m.def("info",
        [](std::string msg, std::string end) {
          Logger().info() << msg << end;
          Logger().info().flush();
        },
        "msg"_a,
        "end"_a = "\n");
  m.def("debug",
        [](std::string msg, std::string end) {
          Logger().debug() << msg << end;
          Logger().debug().flush();
        },
        "msg"_a,
        "end"_a = "\n");
  m.def("error",
        [](std::string msg, std::string end) {
          Logger().error() << msg << end;
          Logger().error().flush();
        },
        "msg"_a,
        "end"_a = "\n");
  m.attr("log_max") = LogMax;
  m.attr("log_default") = LogDefault;
}
