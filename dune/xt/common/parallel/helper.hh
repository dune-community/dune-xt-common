// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014)
//   Rene Milk       (2014 - 2015)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_PARALLEL_HELPER_HH
#define DUNE_XT_COMMON_PARALLEL_HELPER_HH

#include <type_traits>

#include <dune/common/parallel/collectivecommunication.hh>

#if HAVE_DUNE_ISTL
#include <dune/istl/paamg/pinfo.hh>
#endif

namespace Dune {
namespace XT {

//! marker for sequential in MPI-enabled solver stuffs
struct SequentialCommunication
#if HAVE_DUNE_ISTL
    : public Dune::Amg::SequentialInformation
#endif
{
};

template <class GridCommImp>
struct UseParallelCommunication
{
#if HAVE_MPI && HAVE_DUNE_ISTL
  static constexpr bool value = std::is_same<GridCommImp, CollectiveCommunication<MPI_Comm>>::value;
#else
  static constexpr bool value = false;
#endif
};

/**
 * \brief calls MPI_Abort if enable-parallel, noop otherwise
 * \returns MPI_Abort if enable-parallel, 1 otherwise
 **/
int abort_all_mpi_processes();

} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PARALLEL_HELPER_HH
