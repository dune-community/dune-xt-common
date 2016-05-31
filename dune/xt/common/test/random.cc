// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include <dune/xt/common/test/main.hxx>

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/densevector.hh>

#include <dune/xt/common/random.hh>
#include <dune/xt/common/float_cmp.hh>

using namespace Dune::XT::Common;
using namespace Dune::XT::Test;
using namespace std;

TEST(Init, Random)
{
  typedef RNG_FIELD_TYPE T;
  typedef DefaultRNG<T> RNG;
//  typedef typename VectorAbstraction<T>::S Scalar;
  RNG rng1;
  RNG rng2(init_bound<T>(1));
  RNG rng3(init_bound<T>(1),init_bound<T>(2));
  RNG rng4(init_bound<T>(1),init_bound<T>(2),init_bound<T>(0));
  for(auto& r : std::array<RNG, 4>{{rng1, rng2, rng3, rng4}}) {
    auto DXTC_UNUSED(val) = r();
    //!TODO check val
  }
  DefaultRNG<std::string> str_rng(2);
  std::string rstr = str_rng();
  EXPECT_EQ(rstr.size(), 2);
}

