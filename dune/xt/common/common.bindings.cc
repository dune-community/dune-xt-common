// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)

#include "config.h"

#if HAVE_DUNE_PYBINDXI

#include "bindings.hh"


PYBIND11_PLUGIN(_common)
{
  pybind11::module m("_common", "dune-xt-common");
  DUNE_XT_COMMON_BINDINGS_INITIALIZE(m, "dune.xt.common");

  m.def("_is_debug", []() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
  });

  return m.ptr();
}


#endif // HAVE_DUNE_PYBINDXI
