#ifndef DUNE_STUFF_RANGES_RANGES_HH
#define DUNE_STUFF_RANGES_RANGES_HH

#include <dune/grid/common/gridview.hh>
#include <boost/serialization/static_warning.hpp>

#ifdef HAVE_DUNE_FEM
#include <dune/fem/function/common/discretefunction.hh>
#endif

namespace std {

template <class GridViewTraits>
auto begin(Dune::GridView<GridViewTraits>& view) -> decltype(view.template begin<0>())
{
  return view.template begin<0>();
}

template <class GridViewTraits>
auto end(Dune::GridView<GridViewTraits>& view) -> decltype(view.template end<0>())
{
  return view.template end<0>();
}

template <class GridViewTraits>
auto begin(const Dune::GridView<GridViewTraits>& view) -> decltype(view.template begin<0>())
{
  return view.template begin<0>();
}

template <class GridViewTraits>
auto end(const Dune::GridView<GridViewTraits>& view) -> decltype(view.template end<0>())
{
  return view.template end<0>();
}

#ifdef HAVE_DUNE_FEM

template <class DiscreteFunctionTraits>
auto begin(const Dune::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dbegin())
{
  return func.dbegin();
}

template <class DiscreteFunctionTraits>
auto end(const Dune::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dend())
{
  return func.dend();
}

template <class DiscreteFunctionTraits>
auto begin(Dune::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dbegin())
{
  return func.dbegin();
}

template <class DiscreteFunctionTraits>
auto end(Dune::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dend())
{
  return func.dend();
}

#endif
}

namespace Dune {
namespace Stuff {

//! adapter enabling view usage in range-based for
template <class GridViewType, int codim = 0>
class ViewRange
{
  const GridViewType& view_;

public:
  ViewRange(const GridViewType& view)
    : view_(view)
  {
    BOOST_STATIC_WARNING(codim == 0 && "unnecessary ViewRange usage with codim 0");
  }

  typename GridViewType::template Codim<codim>::Iterator begin() const
  {
    return view_.template begin<codim>();
  }
  typename GridViewType::template Codim<codim>::Iterator end() const
  {
    return view_.template end<codim>();
  }
};

/** adapter enabling intersectionniterator usage in range-based for
 * works for GridParts and GridViews
 */
template <class GridAbstractionType, class EntityType>
class IntersectionRange
{
  const GridAbstractionType& view_;
  const EntityType& entity_;

public:
  IntersectionRange(const GridAbstractionType& view, const EntityType& entity)
    : view_(view)
    , entity_(entity)
  {
  }

  auto begin() const -> decltype(view_.ibegin(entity_))
  {
    return view_.ibegin(entity_);
  }

  auto end() const -> decltype(view_.iend(entity_))
  {
    return view_.iend(entity_);
  }
};

//! use sfinae to switch between gridview/gridpart signature
template <class GridViewType>
IntersectionRange<GridViewType, typename GridViewType::template Codim<0>::Entity>
intersectionRange(const GridViewType& gridview, const typename GridViewType::template Codim<0>::Entity& entity)
{
  return IntersectionRange<GridViewType, typename GridViewType::template Codim<0>::Entity>(gridview, entity);
}

template <class GridPartType>
IntersectionRange<GridPartType, typename GridPartType::template Codim<0>::EntityType>
intersectionRange(const GridPartType& gridpart, const typename GridPartType::template Codim<0>::EntityType& entity)
{
  return IntersectionRange<GridPartType, typename GridPartType::template Codim<0>::EntityType>(gridpart, entity);
}

//! Range adpater for lagrane points from lagrange spaces
template <class DiscreteFunctionspaceType, int faceCodim>
class LagrangePointSetRange
{
  typedef typename DiscreteFunctionspaceType::LagrangePointSetType LagrangePointSetType;
  typedef typename LagrangePointSetType::template Codim<faceCodim>::SubEntityIteratorType SubEntityIteratorType;
  const LagrangePointSetType& lp_set_;
  const int subEntity_;

public:
  /** the template isn't lazyness here, the underlying set is tempalted on it too
   */
  template <class EntityType>
  LagrangePointSetRange(const DiscreteFunctionspaceType& space, const EntityType& entity, const int subEntity)
    : lp_set_(space.lagrangePointSet(entity))
    , subEntity_(subEntity_)
  {
  }

  SubEntityIteratorType begin() const
  {
    return lp_set_.template beginSubEntity<faceCodim>(subEntity_);
  }
  SubEntityIteratorType end() const
  {
    return lp_set_.template endSubEntity<faceCodim>(subEntity_);
  }
};

template <int codim, class DiscreteFunctionspaceType, class EntityType>
LagrangePointSetRange<DiscreteFunctionspaceType, codim>
lagrangePointSetRange(const DiscreteFunctionspaceType& space, const EntityType& entity, const int subEntity)
{
  return LagrangePointSetRange<DiscreteFunctionspaceType, codim>(space, entity, subEntity);
}

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_RANGES_RANGES_HH
/** Copyright (c) 2012, Felix Albrecht, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
