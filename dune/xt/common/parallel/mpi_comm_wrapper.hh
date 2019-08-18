// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   René Fritze    (2018)
//   Tobias Leibner (2018)

#ifndef DUNE_XT_COMMON_MPI_COMM_WRAPPER_HH
#define DUNE_XT_COMMON_MPI_COMM_WRAPPER_HH

#include <dune/common/parallel/mpihelper.hh>

#if HAVE_MPI
#  include <mpi.h>
#  include <mpi4py/mpi4py.h>
// while not strictly HAVE_MPI, only then are they needed
#  include <python/pybindxi/pybind11.h>
#  include <python/pybindxi/stl.h>
#endif

namespace Dune {
namespace XT {
namespace Common {

//! this class is needed because pybind11 cannot directly use the real mpi_comm type
class MPI_Comm_Wrapper
{
  using WrappedComm = MPIHelper::MPICommunicator;

public:
  MPI_Comm_Wrapper(WrappedComm comm = MPIHelper::getCommunicator());
  MPI_Comm_Wrapper& operator=(const WrappedComm comm);
  WrappedComm get() const;

private:
  WrappedComm comm_;
};

} // namespace Common
} // namespace XT
} // namespace Dune

#if HAVE_MPI

namespace pybind11 {
namespace detail {

template <>
class type_caster<Dune::XT::Common::MPI_Comm_Wrapper>
{
public:
  // Define this->value of type MPICommWrapper
  PYBIND11_TYPE_CASTER(Dune::XT::Common::MPI_Comm_Wrapper, _("MPI_Comm_Wrapper"));

  // Python to C++
  bool load(handle src, bool)
  {
    // Simplified version of isinstance(src, mpi4py.MPI.Comm) - avoids segfault
    // when pybind11 tries to convert some other random type to MPICommWrapper
    if (not hasattr(src, "Allgather"))
      return false;
    import_mpi4py();
    value = Dune::XT::Common::MPI_Comm_Wrapper(*PyMPIComm_Get(src.ptr()));
    return true;
  }

  // C++ to Python
  static handle
  cast(Dune::XT::Common::MPI_Comm_Wrapper src, pybind11::return_value_policy /*policy*/, handle /*parent*/)
  {
    import_mpi4py();
    return pybind11::handle(PyMPIComm_New(src.get()));
  }

  operator Dune::XT::Common::MPI_Comm_Wrapper()
  {
    return this->value;
  }
};
} // namespace detail
} // namespace pybind11
#endif // HAVE_MPI

#endif // DUNE_XT_COMMON_MPI_COMM_WRAPPER_HH
