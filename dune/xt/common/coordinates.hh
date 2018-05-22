// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Andreas Buhr    (2014)
//   Felix Schindler (2012 - 2018)
//   Rene Milk       (2010 - 2018)
//   Sven Kaulmann   (2013)
//   Tobias Leibner  (2014, 2017)

#ifndef DUNE_XT_COMMON_COORDINATES_HH
#define DUNE_XT_COMMON_COORDINATES_HH


#include <dune/xt/common/disable_warnings.hh>
#include <boost/geometry.hpp>
#include <dune/xt/common/reenable_warnings.hh>

#include <dune/xt/common/fvector.hh>

namespace Dune {
namespace XT {
namespace Common {


/** Converts from (x, y, z) to (theta, phi) on the unit sphere s.t.
 * (x, y, z) = (sin(theta) cos(phi), sin(theta) sin(phi), cos(theta))
 * with 0 \leq \theta \leq \pi and 0 \leq \varphi < 2\pi. **/
template <class DomainFieldType>
class CoordinateConverter
{
  typedef typename boost::geometry::model::point<DomainFieldType, 3, typename boost::geometry::cs::cartesian>
      BoostCartesianCoordType;
  typedef typename boost::geometry::model::
      point<DomainFieldType, 2, typename boost::geometry::cs::spherical<boost::geometry::radian>>
          BoostSphericalCoordType;

public:
  typedef FieldVector<DomainFieldType, 3> CartesianCoordType;
  typedef FieldVector<DomainFieldType, 2> SphericalCoordType;

  static SphericalCoordType to_spherical(const CartesianCoordType& x)
  {
    BoostCartesianCoordType x_boost(x[0], x[1], x[2]);
    BoostSphericalCoordType x_spherical_boost;
    boost::geometry::transform(x_boost, x_spherical_boost);
    return SphericalCoordType{boost::geometry::get<1>(x_spherical_boost), boost::geometry::get<0>(x_spherical_boost)};
  }

  static CartesianCoordType to_cartesian(const SphericalCoordType& x_spherical)
  {
    BoostSphericalCoordType x_spherical_boost(x_spherical[1], x_spherical[0]);
    BoostCartesianCoordType x_boost;
    boost::geometry::transform(x_spherical_boost, x_boost);
    return CartesianCoordType{
        boost::geometry::get<0>(x_boost), boost::geometry::get<1>(x_boost), boost::geometry::get<2>(x_boost)};
  }
};


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_COORDINATES_HH
