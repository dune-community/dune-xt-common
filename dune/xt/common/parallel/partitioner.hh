// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2014 - 2015)

#ifndef DUNE_XT_COMMON_PARALLEL_PARTITIONER_HH
#define DUNE_XT_COMMON_PARALLEL_PARTITIONER_HH

#include <cstddef>

namespace Dune {
namespace Stuff {

/** \brief Partition that assigns each codim-0 entity in a \ref IndexSet a unique partition number,
 * its index in the set
 *
 * usable with \ref Dune::SeedListPartitioning for example \ref Dune::PartitioningInterface
 **/
template <class GridViewType>
struct IndexSetPartitioner
{
  typedef typename GridViewType::IndexSet IndexSetType;
  typedef typename GridViewType::template Codim<0>::Entity EntityType;
  explicit IndexSetPartitioner(const IndexSetType& index_set)
    : index_set_(index_set)
  {
  }

  std::size_t partition(const EntityType& e) const
  {
    return index_set_.index(e);
  }

  std::size_t partitions() const
  {
    return index_set_.size(0);
  }

private:
  const IndexSetType& index_set_;
};
}
}

#endif // DUNE_XT_COMMON_PARALLEL_PARTITIONER_HH
