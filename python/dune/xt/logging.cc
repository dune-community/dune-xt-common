// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)
//   Rene Milk       (2018)

#include "config.h"

#include <string>
#include <vector>

#include <dune/common/parallel/mpihelper.hh>
#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>

#include <python/dune/xt/common/bindings.hh>

#include <dune/pybindxi/pybind11.h>


PYBIND11_MODULE(_logging, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;
  using namespace Dune::XT::Common;

  Dune::XT::Common::bindings::addbind_exceptions(m);

  Dune::XT::Common::bindings::add_initialization(m, "dune.xt.logging");

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
