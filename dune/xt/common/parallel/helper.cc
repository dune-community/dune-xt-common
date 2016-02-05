// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016)
//   Rene Milk       (2011 - 2012, 2014 - 2015)

#include <config.h>

#include "helper.hh"
#include <dune/common/parallel/mpihelper.hh>

int Dune::XT::abort_all_mpi_processes()
{
#if HAVE_MPI
  if (MPIHelper::getCollectiveCommunication().size() > 1)
    return MPI_Abort(MPIHelper::getCommunicator(), 1);
  else
#endif
    return 1;
}
