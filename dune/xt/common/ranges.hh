// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2017)
//   Rene Milk       (2012 - 2016)
//   Sven Kaulmann   (2012 - 2013)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_RANGES_HH
#define DUNE_XT_COMMON_RANGES_HH

#include <type_traits>

#include <boost/serialization/static_warning.hpp>
#include <boost/iterator/iterator_facade.hpp>

#if HAVE_DUNE_FEM
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/gridpart/common/gridpart.hh>
#include <dune/fem/space/lagrange/lagrangepoints.hh>
#endif // HAVE_DUNE_FEM

#include <dune/xt/common/math.hh>

namespace Dune {

#if HAVE_DUNE_FEM

namespace Fem {

template <class DiscreteFunctionTraits>
auto begin(const Dune::Fem::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dbegin())
{
  return func.dbegin();
}

template <class DiscreteFunctionTraits>
auto end(const Dune::Fem::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dend())
{
  return func.dend();
}

template <class DiscreteFunctionTraits>
auto begin(Dune::Fem::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dbegin())
{
  return func.dbegin();
}

template <class DiscreteFunctionTraits>
auto end(Dune::Fem::DiscreteFunctionInterface<DiscreteFunctionTraits>& func) -> decltype(func.dend())
{
  return func.dend();
}

} // namespace Fem

#endif // HAVE_DUNE_FEM

namespace XT {
namespace Common {

#if HAVE_DUNE_FEM

//! Range adapter for lagrange points from lagrange spaces
template <class GridPartType, int order, int faceCodim>
class LagrangePointSetRange
{
  typedef Dune::Fem::LagrangePointSet<GridPartType, order> LagrangePointSetType;
  typedef typename LagrangePointSetType::template Codim<faceCodim>::SubEntityIteratorType SubEntityIteratorType;
  const LagrangePointSetType& lp_set_;
  const size_t subEntity_;

public:
  /** the template isn't lazyness here, the underlying set is templated on it too
   */
  template <class DiscreteFunctionspaceType, class EntityType>
  LagrangePointSetRange(const DiscreteFunctionspaceType& space, const EntityType& entity, const size_t subEntity)
    : lp_set_(space.lagrangePointSet(entity))
    , subEntity_(subEntity)
  {
  }

  LagrangePointSetRange(const LagrangePointSetType& lp_set, const size_t subEntity)
    : lp_set_(lp_set)
    , subEntity_(subEntity)
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

template <size_t codim, class DiscreteFunctionspaceType, class EntityType>
LagrangePointSetRange<typename DiscreteFunctionspaceType::GridPartType,
                      DiscreteFunctionspaceType::polynomialOrder,
                      codim>
lagrange_point_set_range(const DiscreteFunctionspaceType& space, const EntityType& entity, const size_t subEntity)
{
  return LagrangePointSetRange<typename DiscreteFunctionspaceType::GridPartType,
                               DiscreteFunctionspaceType::polynomialOrder,
                               codim>(space, entity, subEntity);
}

template <class LgPointSetType, size_t codim = 1>
LagrangePointSetRange<typename LgPointSetType::GridPartType, LgPointSetType::polynomialOrder, codim>
lagrange_point_set_range(const LgPointSetType& lpset, const size_t subEntity)
{
  return LagrangePointSetRange<typename LgPointSetType::GridPartType, LgPointSetType::polynomialOrder, codim>(
      lpset, subEntity);
}

#endif // HAVE_DUNE_FEM

//! get a vector with values in [start : increment : end)
template <class T, class sequence = std::vector<T>>
typename std::enable_if<!std::is_enum<T>::value, sequence>::type
value_range(const T start, const T end, const T increment = Epsilon<T>::value)
{
  // sadly, no overloaded version of std::abs is available for
  // unsigned long long, so we compute the absolute value of increment
  // ourselves
  const auto incrementAbs = abs(increment);
  assert(incrementAbs > 0);
  sequence ret(typename sequence::size_type(((end > start) ? end - start : start - end) / incrementAbs), start);
  typename sequence::size_type i = 0;
  std::generate(std::begin(ret), std::end(ret), [&]() { return T(start + (increment * i++)); });
  return ret;
}

//! signature for enumeration Types T
template <class T, class sequence = std::vector<typename internal::Absretval<T>::type>>
typename std::enable_if<std::is_enum<T>::value, sequence>::type value_range(
    const T start,
    const T end,
    const typename internal::Absretval<T>::type increment = Epsilon<typename internal::Absretval<T>::type>::value)
{
  typedef typename internal::Absretval<T>::type R;
  return value_range(static_cast<R>(start), static_cast<R>(end), increment);
}

//! get a vector with values in [0 : Epsilon<T> : end)
template <class T, class sequence = std::vector<T>>
typename std::enable_if<!std::is_enum<T>::value, sequence>::type value_range(const T end)
{
  return value_range(T(0), end);
}

//! get a vector with values in [0 : Epsilon<T> : end)
template <class T, class sequence = std::vector<typename internal::Absretval<T>::type>>
typename std::enable_if<std::is_enum<T>::value, sequence>::type value_range(const T end)
{
  return value_range(T(0), end);
}

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_RANGES_HH
