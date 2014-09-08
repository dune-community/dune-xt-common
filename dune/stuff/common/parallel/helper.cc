// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include <config.h>

#include "helper.hh"
#include <dune/common/parallel/mpihelper.hh>

int Dune::Stuff::abort_all_mpi_processes()
{
#if HAVE_MPI
  return MPI_Abort(MPIHelper::getCommunicator());
#endif
  return 1;
}
