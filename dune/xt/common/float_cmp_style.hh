// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)
//   Rene Milk       (2015 - 2016, 2018)

#ifndef DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH
#define DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH

#include <type_traits>
#include <complex>

#include <dune/xt/common/math.hh> // <- This include needs to be before the one from dune-common, otherwise
#include <dune/common/float_cmp.hh> //  std::abs(long unsinged int) is indefined in dune-common!

#include <dune/xt/common/type_traits.hh>

namespace Dune {
namespace XT {
namespace Common {
namespace FloatCmp {

enum class Style
{
  numpy,
  relativeWeak,
  relativeStrong,
  absolute,
  defaultStyle = numpy
};

namespace internal {

template<Style style>
struct ConvertStyle;

template<>
struct ConvertStyle<Style::relativeWeak>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeWeak;
};

template<>
struct ConvertStyle<Style::relativeStrong>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeStrong;
};

template<>
struct ConvertStyle<Style::absolute>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::absolute;
};

} // namespace internal

} // namespace FloatCmp
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH
