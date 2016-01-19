// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012 - 2014)
//   Rene Milk       (2011 - 2012, 2014 - 2015)

#include <config.h>

#include "helper.hh"
#include <dune/common/parallel/mpihelper.hh>

int Dune::Stuff::abort_all_mpi_processes()
{
#if HAVE_MPI
  if (MPIHelper::getCollectiveCommunication().size() > 1)
    return MPI_Abort(MPIHelper::getCommunicator(), 1);
  else
#endif
    return 1;
}
