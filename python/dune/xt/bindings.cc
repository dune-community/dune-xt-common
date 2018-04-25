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

#include <python/dune/xt/common/bindings.hh>

#include <string>
#include <vector>

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
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/timedlogging.hh>


PYBIND11_MODULE(_common, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

  Dune::XT::Common::bindings::addbind_exceptions(m);

  Dune::XT::Common::bindings::add_initialization(m, "dune.xt.common");

  m.def("_is_debug", []() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
  });
}
