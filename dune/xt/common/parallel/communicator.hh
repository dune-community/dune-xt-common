// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Tobias Leibner  (2019)

#ifndef DUNE_XT_COMMON_PARALLEL_COMMUNICATOR_HH
#define DUNE_XT_COMMON_PARALLEL_COMMUNICATOR_HH

#include <dune/common/parallel/communicator.hh>

#if !HAVE_MPI

namespace Dune {


// dune-common is missing comparison operators for No_Comm
bool operator==(const No_Comm& /*lhs*/, const No_Comm& /*rhs*/)
{
  return true;
}
bool operator!=(const No_Comm& lhs, const No_Comm& rhs)
{
  return !(lhs == rhs);
}


} // namespace Dune

#endif // !HAVE_MPI

#endif // DUNE_XT_COMMON_PARALLEL_COMMUNICATOR_HH
