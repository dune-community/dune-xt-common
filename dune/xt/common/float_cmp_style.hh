// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2015)

#ifndef DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH
#define DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH

#include <type_traits>
#include <complex>

#include <dune/common/float_cmp.hh>
#include <dune/stuff/common/type_utils.hh>

namespace Dune {
namespace Stuff {
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

template <Style style>
struct ConvertStyle;

template <>
struct ConvertStyle<Style::relativeWeak>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeWeak;
};

template <>
struct ConvertStyle<Style::relativeStrong>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::relativeStrong;
};

template <>
struct ConvertStyle<Style::absolute>
{
  static const Dune::FloatCmp::CmpStyle value = Dune::FloatCmp::CmpStyle::absolute;
};

} // namespace internal

} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_XT_COMMON_FLOAT_CMP_STYLE_HH
