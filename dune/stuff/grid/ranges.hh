#ifndef DUNE_STUFF_RANGES_RANGES_HH
#define DUNE_STUFF_RANGES_RANGES_HH

#include <dune/grid/common/gridview.hh>

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
}

namespace Dune {
namespace Stuff {
namespace Grid {

//! adapter enabling view usage in range-based for
template <class GridViewType, int codim = 0>
class ViewRange
{
  const GridViewType& view_;

public:
  ViewRange(const GridViewType& view) DUNE_DEPRECATED_MSG("completely obsoleted with specialized std::begin/end")
    : view_(view)
  {
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

//! adapter enabling intersectionniterator usage in range-based for
template <class GridViewType>
class IntersectionRange
{
  typedef typename GridViewType::template Codim<0>::Entity EntityType;
  const GridViewType& view_;
  const EntityType& entity_;

public:
  IntersectionRange(const GridViewType& view, const EntityType& entity)
    : view_(view)
    , entity_(entity)
  {
  }

  typename GridViewType::IntersectionIterator begin() const
  {
    return view_.ibegin(entity_);
  }
  typename GridViewType::IntersectionIterator end() const
  {
    return view_.iend(entity_);
  }
};

} // namespace Grid
} // namespace Stud
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
