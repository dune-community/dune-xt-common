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

#ifndef DUNE_XT_COMMON_EXCEPTIONS_BINDINGS_HH
#define DUNE_XT_COMMON_EXCEPTIONS_BINDINGS_HH
#if HAVE_DUNE_PYBINDXI

#include <string>

#include <boost/exception/all.hpp>

#if HAVE_TBB
#include <tbb/tbb_exception.h>
#endif

#include <dune/pybindxi/pybind11.h>

#include <dune/xt/common/exceptions.hh>

namespace Dune {
namespace XT {
namespace Common {
namespace bindings {


void addbind_exceptions(pybind11::module& m)
{
  namespace py = pybind11;

  static py::exception<std::exception> std_exc(m, "StdError");
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
    } catch (const std::exception& e) {
      std_exc(e.what());
    }
  });
} // ... addbind_exceptions(...)


} // namespace bindings
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // HAVE_DUNE_PYBINDXI
#endif // DUNE_XT_COMMON_EXCEPTIONS_BINDINGS_HH
