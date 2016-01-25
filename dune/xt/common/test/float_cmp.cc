// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015 - 2016)
//   Rene Milk       (2015)

#define DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS 1

#include <dune/xt/common/test/main.hxx>

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#include <dune/xt/common/float_cmp.hh>
#include <dune/xt/common/test/float_cmp.hh>
#include <dune/xt/common/fvector.hh>

using namespace Dune;
using XT::Common::create;
using namespace XT::Common::FloatCmp;
using XT::Common::VectorAbstraction;

struct FloatCmpTest : public testing::Test
{
  typedef TESTTYPE V;
  static const size_t s_size = VectorAbstraction<V>::has_static_size ? VectorAbstraction<V>::static_size : VECSIZE;

  typedef typename VectorAbstraction<V>::ScalarType S;
  typedef typename VectorAbstraction<V>::RealType R;

  FloatCmpTest()
    : zero(create<V>(s_size, create<S>(0, 0)))
    , one(create<V>(s_size, create<S>(0, 1)))
    , epsilon(create<V>(s_size, DefaultEpsilon<S>::value()))
    , test_config(DXTC_CONFIG.sub("test_common_float_cmp"))
  {
  }

  const V zero;
  const V one;
  const V epsilon;
  const typename XT::Common::Configuration test_config;

  void check_eq()
  {
    XT_TEST_DXTC_EXPECT_FLOAT_EQ(zero, zero);

    if (test_config["comparison_is_relative"] == "true") {
      EXPECT_FALSE(FLOATCMP_EQ(zero, 0.9 * epsilon));
      EXPECT_FALSE(FLOATCMP_EQ(one, one + 1.1 * epsilon));
    } else {
      XT_TEST_DXTC_EXPECT_FLOAT_EQ(zero, 0.9 * epsilon);
      XT_TEST_DXTC_EXPECT_FLOAT_EQ(one, one + 1.1 * epsilon);
    }

    if (test_config["style_is_absolute"] == "true")
      XT_TEST_DXTC_EXPECT_FLOAT_EQ(zero, 1.1 * epsilon);
    else
      EXPECT_FALSE(FLOATCMP_EQ(zero, 1.1 * epsilon));

    EXPECT_FALSE(FLOATCMP_EQ(zero, one));
    XT_TEST_DXTC_EXPECT_FLOAT_EQ(one, one + 0.9 * epsilon);
    EXPECT_FALSE(FLOATCMP_EQ(one, 2. * one));
  }

  void check_ne()
  {
    EXPECT_FALSE(FLOATCMP_NE(zero, zero));

    if (test_config["comparison_is_relative"] == "true") {
      EXPECT_TRUE(FLOATCMP_NE(zero, 0.9 * epsilon));
      EXPECT_TRUE(FLOATCMP_NE(one, one + 1.1 * epsilon));
    } else {
      EXPECT_FALSE(FLOATCMP_NE(zero, 0.9 * epsilon));
      EXPECT_FALSE(FLOATCMP_NE(one, one + 1.1 * epsilon));
    }

    if (test_config["style_is_absolute"] == "true")
      EXPECT_FALSE(FLOATCMP_NE(zero, 1.1 * epsilon));
    else
      EXPECT_TRUE(FLOATCMP_NE(zero, 1.1 * epsilon));

    EXPECT_TRUE(FLOATCMP_NE(zero, one));
    EXPECT_FALSE(FLOATCMP_NE(one, one + 0.9 * epsilon));
    EXPECT_TRUE(FLOATCMP_NE(one, 2. * one));
  }

  void check_gt()
  {
    EXPECT_FALSE(FLOATCMP_GT(zero, zero));

    if (test_config["comparison_is_relative"] == "true") {
      EXPECT_TRUE(FLOATCMP_GT(0.9 * epsilon, zero));
      EXPECT_TRUE(FLOATCMP_GT(one + 1.1 * epsilon, one));
    } else {
      EXPECT_FALSE(FLOATCMP_GT(0.9 * epsilon, zero));
      EXPECT_FALSE(FLOATCMP_GT(one + 1.1 * epsilon, one));
    }

    if (test_config["style_is_absolute"] == "true")
      EXPECT_FALSE(FLOATCMP_GT(1.1 * epsilon, zero));
    else
      EXPECT_TRUE(FLOATCMP_GT(1.1 * epsilon, zero));

    EXPECT_TRUE(FLOATCMP_GT(one, zero));
    EXPECT_FALSE(FLOATCMP_GT(one + 0.9 * epsilon, one));
    EXPECT_TRUE(FLOATCMP_GT(2. * one, one));
  }

  void check_lt()
  {
    EXPECT_FALSE(FLOATCMP_LT(zero, zero));

    if (test_config["comparison_is_relative"] == "true") {
      EXPECT_TRUE(FLOATCMP_LT(zero, 0.9 * epsilon));
      EXPECT_TRUE(FLOATCMP_LT(one, one + 1.1 * epsilon));
    } else {
      EXPECT_FALSE(FLOATCMP_LT(zero, 0.9 * epsilon));
      EXPECT_FALSE(FLOATCMP_LT(one, one + 1.1 * epsilon));
    }

    if (test_config["style_is_absolute"] == "true")
      EXPECT_FALSE(FLOATCMP_LT(zero, 1.1 * epsilon));
    else
      EXPECT_TRUE(FLOATCMP_LT(zero, 1.1 * epsilon));

    EXPECT_TRUE(FLOATCMP_LT(zero, one));
    EXPECT_FALSE(FLOATCMP_LT(one, one + 0.9 * epsilon));
    EXPECT_TRUE(FLOATCMP_LT(one, 2. * one));
  }

  void check_ge()
  {
    EXPECT_TRUE(FLOATCMP_GE(zero, zero));
    EXPECT_TRUE(FLOATCMP_GE(0.9 * epsilon, zero));
    EXPECT_TRUE(FLOATCMP_GE(1.1 * epsilon, zero));
    EXPECT_TRUE(FLOATCMP_GE(one, zero));
    EXPECT_TRUE(FLOATCMP_GE(one + 0.9 * epsilon, one));
    EXPECT_TRUE(FLOATCMP_GE(one + 1.1 * epsilon, one));
    EXPECT_TRUE(FLOATCMP_GE(2. * one, one));
  }

  void check_le()
  {
    EXPECT_TRUE(FLOATCMP_LE(zero, zero));
    EXPECT_TRUE(FLOATCMP_LE(zero, 0.9 * epsilon));
    EXPECT_TRUE(FLOATCMP_LE(zero, 1.1 * epsilon));
    EXPECT_TRUE(FLOATCMP_LE(zero, one));
    EXPECT_TRUE(FLOATCMP_LE(one, one + 0.9 * epsilon));
    EXPECT_TRUE(FLOATCMP_LE(one, one + 1.1 * epsilon));
    EXPECT_TRUE(FLOATCMP_LE(one, 2. * one));
  }
}; // struct FloatCmpBase


TEST_F(FloatCmpTest, eq)
{
  this->check_eq();
}

TEST_F(FloatCmpTest, ne)
{
  this->check_ne();
}

TEST_F(FloatCmpTest, gt)
{
  this->check_gt();
}

TEST_F(FloatCmpTest, lt)
{
  this->check_lt();
}

TEST_F(FloatCmpTest, ge)
{
  this->check_ge();
}

TEST_F(FloatCmpTest, le)
{
  this->check_le();
}
