#ifndef DUNE_STUFF_COMMON_PARALLEL_PARTITIONER_HH
#define DUNE_STUFF_COMMON_PARALLEL_PARTITIONER_HH

#include <dune/grid/common/gridview.hh>

namespace Dune {
namespace Stuff {

template <class GridViewType>
struct IndexSetPartitioner
{
  typedef typename GridViewType::IndexSet IndexSetType;
  typedef typename GridViewType::template Codim<0>::Entity EntityType;
  IndexSetPartitioner(const IndexSetType& index_set)
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

#endif // DUNE_STUFF_COMMON_PARALLEL_PARTITIONER_HH
