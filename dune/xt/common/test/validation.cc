// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014, 2016)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include <dune/xt/common/test/main.hxx>

#include <dune/xt/common/validation.hh>
#include <dune/xt/common/type_traits.hh>
#include <dune/xt/common/math.hh>
#include <dune/xt/common/random.hh>
#include <dune/common/tuples.hh>
#include <dune/common/tupleutility.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/bigunsignedint.hh>
#include <limits>
#include <iostream>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>

using namespace Dune::XT::Common;
using namespace Dune::XT::Test;

struct ValidationTest : public testing::Test
{
  typedef FIELD_TYPE T;
  typedef DefaultRNG<T> RNGType;
  static const T eps;
  /** for some weird reason my compiler thinks ValidationTest is an abstract class
   * if I don't implement "void TestBody();"
   * \see common_math.cc testcases for why I think it's weird
   **/
  void all()
  {
    using namespace boost::assign;
    {
      typedef VectorAbstraction<T>::S S;
      const auto scalar_eps = Epsilon<S>::value;
      const T lower = init_bound<T>(std::numeric_limits<S>::min() / 6 + scalar_eps);
      const T upper = init_bound<T>(std::numeric_limits<S>::max() / 6 - scalar_eps);

      for (auto&& arg : std::array<T, 3>{{lower, T(0), upper}}) {
        test(lower, upper, arg);
      }
      RNGType rng(lower, upper);
      boost::array<T, 10> ar = list_of<T>().repeat_fun(9, rng);
      ValidateInList<T, boost::array<T, 10>> validator(ar);
      for (T t : ar) {
        EXPECT_TRUE(validator(t));
      }
      std::vector<T> a;
      EXPECT_FALSE(ValidateInList<T>(a)(rng()));
    }
    std::cout << "\t\tfixed interval" << std::endl;
    {
      const T lower = init_bound<T>(0);
      const T upper = init_bound<T>(2);
      const T arg = init_bound<T>(1);
      test(lower, upper, arg);
      EXPECT_FALSE(ValidateLess<T>(upper)(lower));
      EXPECT_FALSE(ValidateGreater<T>(lower)(upper));
      EXPECT_FALSE(ValidateGreaterOrEqual<T>(lower)(upper + Epsilon<T>::value));
    }
    std::cout << "\t\tdone." << std::endl;
  }

  void test(const T lower, const T upper, const T arg) const
  {
    const T clamped_arg = clamp(arg, T(lower + eps), T(upper - eps));
    EXPECT_TRUE(ValidateAny<T>()(arg));
    EXPECT_TRUE(ValidateLess<T>(clamped_arg)(upper));
    EXPECT_TRUE(ValidateGreaterOrEqual<T>(arg)(lower));
    EXPECT_TRUE(ValidateGreater<T>(clamped_arg)(lower));
    EXPECT_TRUE(ValidateInterval<T>(lower, upper)(arg));
    EXPECT_FALSE(ValidateNone<T>()(arg));
  }
};
const FIELD_TYPE ValidationTest::eps = Epsilon<T>::value;

TEST_F(ValidationTest, All)
{
  this->all();
}
