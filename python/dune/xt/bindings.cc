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
#include <boost/exception/all.hpp>

#if HAVE_TBB
#include <tbb/tbb_exception.h>
#endif

#include <dune/pybindxi/pybind11.h>

#include <python/dune/xt/common/exceptions.bindings.hh>
#include <python/dune/xt/common/fvector.hh>
#include <python/dune/xt/common/fmatrix.hh>
#include <python/dune/xt/common/configuration.hh>
#include <dune/xt/common/parallel/mpi_comm_wrapper.hh>
#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/string.hh>
#include <dune/xt/common/timedlogging.hh>

#include <dune/common/parallel/mpihelper.hh>
#include <dune/istl/owneroverlapcopy.hh>

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/timedlogging.hh>
#include <dune/xt/common/parallel/helper.hh>

void addbind_mpistuff(pybind11::module& m)
{
  namespace py = pybind11;
  using pybind11::operator""_a;
  typedef Dune::CollectiveCommunication<Dune::MPIHelper::MPICommunicator> Comm;
  py::class_<Comm> cls(m, "CollectiveCommunication", "CollectiveCommunication");
  cls.def(py::init([](Dune::XT::Common::MPI_Comm_Wrapper comm) { return std::make_unique<Comm>(comm.get()); }),
          "comm"_a = Dune::XT::Common::MPI_Comm_Wrapper());

  cls.def_property_readonly("rank", &Comm::rank);
  cls.def_property_readonly("size", &Comm::size);

  cls.def("barrier", &Comm::barrier);

  cls.def("min", [](const Comm& self, double x) { return self.min(x); }, "x"_a);
  cls.def("max", [](const Comm& self, double x) { return self.max(x); }, "x"_a);
  cls.def("sum", [](const Comm& self, double x) { return self.sum(x); }, "x"_a);

  using SeqComm = Dune::XT::SequentialCommunication;
  py::class_<SeqComm> seqcomm(m, "SequentialCommunication", "SequentialCommunication");
  seqcomm.def(py::init());
#if HAVE_MPI
  using ParaComm = Dune::OwnerOverlapCopyCommunication<unsigned long, int>;
  py::class_<ParaComm> paracomm(m, "OwnerOverlapCopyCommunication", "OwnerOverlapCopyCommunication");
#endif
  m.def("abort_all_mpi_processes", &Dune::XT::abort_all_mpi_processes);
}

void addbind_exceptions(pybind11::module& m)
{
  namespace py = pybind11;

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
} // ... addbind_exceptions(...)

PYBIND11_MODULE(_common, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

  addbind_exceptions(m);
  addbind_mpistuff(m);

  Dune::XT::Common::bindings::add_initialization(m, "dune.xt.common");

  m.def("_is_debug", []() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
  });
}
