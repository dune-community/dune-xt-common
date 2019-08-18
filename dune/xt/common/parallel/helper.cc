// This file is part of the dune-xt project:
//   https://github.com/dune-community/dune-xt
// Copyright 2009-2018 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016 - 2017)
//   René Fritze     (2011 - 2012, 2014 - 2016, 2018)

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
