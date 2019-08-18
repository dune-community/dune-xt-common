// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   René Fritze (2018)

#include "config.h"

#include <string>
#include <vector>

#include <python/pybindxi/pybind11.h>
#include <python/pybindxi/stl.h>
#include <python/pybindxi/iostream.h>

#include <python/dune/xt/common/bindings.hh>
#include <dune/xt/common/python.hh>
#include <dune/xt/common/timings.hh>
#include <python/dune/xt/common/exceptions.bindings.hh>
#include <dune/common/parallel/mpihelper.hh>


PYBIND11_MODULE(_timings, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;
  using namespace Dune::XT::Common;

  bindings::try_register(m, [](auto& m_) {
    py::class_<Timings>(m_, "Timings")
        .def("start", &Timings::start, "set this to begin a named section")
        .def("reset", py::overload_cast<std::string>(&Timings::reset), "set elapsed time back to 0 for section_name")
        .def("reset", py::overload_cast<>(&Timings::reset), "set elapsed time back to 0 for section_name")
        .def("stop", py::overload_cast<std::string>(&Timings::stop), "stop all timer for given section only")
        .def("stop", py::overload_cast<>(&Timings::stop), "stop all running timers")
        .def("walltime", &Timings::walltime, "get runtime of section in milliseconds")
        //! TODO this actually accepts an ostream
        .def("output_simple", [](Timings& self) { self.output_simple(); }, "outputs per-rank csv-file")
        .def("output_per_rank", &Timings::output_per_rank, "outputs walltime only")
        //! TODO this actually accepts an MPICOMM and an ostream too
        .def("output_all_measures",
             [](Timings& self) { self.output_all_measures(); },
             "outputs per rank and global averages of all measures");
    m_.def("instance", &timings, py::return_value_policy::reference);
  });
}
