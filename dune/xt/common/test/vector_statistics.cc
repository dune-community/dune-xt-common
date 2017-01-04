// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Rene Milk       (2017)
#include <dune/xt/common/test/main.hxx>

#include <dune/xt/common/type_traits.hh>
#include <dune/xt/common/vector_statistics.hh>

using namespace Dune::XT::Common;
using namespace std;

TEST(STDDEV, Vector)
{
  const auto vec = VectorAbstraction<VECTOR_TYPE>::create(VECTOR_SIZE, 0);
  EXPECT_DOUBLE_EQ(standard_deviation(vec), 0);
}
