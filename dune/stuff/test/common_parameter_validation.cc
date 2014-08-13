// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/random.hh>
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

using namespace Dune::Stuff::Common;

typedef testing::Types<double, float, // Dune::bigunsignedint,
                       int, unsigned int, unsigned long, long long, char> MathTestTypes;

template <class T>
struct ValidationTest : public testing::Test
{
  typedef DefaultRNG<T> RNGType;
  /** for some weird reason my compiler thinks ValidationTest is an abstract class
   * if I don't implement "void TestBody();"
   * \see common_math.cc testcases for why I think it's weird
   **/
  void TestBody()
  {
    using namespace boost::assign;
    const int samples = 100000;
    std::cout << "\tTesting Validators for type " << Typename<T>::value() << "\n\t\t" << samples
              << " random numbers ..." << std::endl;
    {
      const T lower = std::numeric_limits<T>::min();
      const T upper = std::numeric_limits<T>::max();
      RNGType rng;
      for (int i = samples; i > 0; --i) {
        const T arg = rng();
        test(lower, upper, arg);
      }
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
      const T lower = T(0);
      const T upper = T(2);
      const T arg = T(1);
      test(lower, upper, arg);
      EXPECT_FALSE(ValidateLess<T>(upper)(lower));
      EXPECT_FALSE(ValidateGreater<T>(lower)(upper));
      EXPECT_FALSE(ValidateGreaterOrEqual<T>(lower)(upper + Epsilon<T>::value));
    }
    std::cout << "\t\tdone." << std::endl;
  }

  void test(const T lower, const T upper, const T arg) const
  {
    const T eps         = Epsilon<T>::value;
    const T clamped_arg = clamp(arg, T(lower + eps), T(upper - eps));
    EXPECT_TRUE(ValidateAny<T>()(arg));
    EXPECT_TRUE(ValidateLess<T>(clamped_arg)(upper));
    EXPECT_TRUE(ValidateGreaterOrEqual<T>(arg)(lower));
    EXPECT_TRUE(ValidateGreater<T>(clamped_arg)(lower));
    EXPECT_TRUE(ValidateInterval<T>(lower, upper)(arg));
    EXPECT_FALSE(ValidateNone<T>()(arg));
  }
};

TYPED_TEST_CASE(ValidationTest, MathTestTypes);
TYPED_TEST(ValidationTest, All)
{
  ValidationTest<TypeParam> k;
  k.TestBody();
}

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}
