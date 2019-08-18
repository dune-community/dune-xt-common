// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2019)

#include "config.h"

#include <exception>

#include <boost/exception/all.hpp>

#if HAVE_TBB
#  include <tbb/tbb_exception.h>
#endif

#include <dune/common/exceptions.hh>

#include <python/pybindxi/pybind11.h>
#include <python/pybindxi/stl.h>


PYBIND11_MODULE(_exceptions, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

#if HAVE_TBB
  static py::exception<tbb::tbb_exception> tbb_exc(m, "TbbError");
#endif
  static py::exception<boost::exception> boost_exc(m, "BoostError");
  static py::exception<Dune::Exception> dune_exc(m, "DuneError");

  py::register_exception_translator([](std::exception_ptr p) {
    try {
      if (p)
        std::rethrow_exception(p);
    } catch (const boost::exception& e) {
      boost_exc(boost::diagnostic_information_what(e));
    } catch (const Dune::Exception& e) {
      dune_exc(e.what());
#if HAVE_TBB
    } catch (const tbb::tbb_exception& e) {
      tbb_exc((std::string(e.name()) + ": " + e.what()).c_str());
#endif
    }
  });
}
