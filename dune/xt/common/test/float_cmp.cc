// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2015 - 2016)
//   Rene Milk       (2015)

#define DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS 1

#include <dune/xt/common/test/main.hxx>

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

#include <dune/xt/common/float_cmp.hh>
#include <dune/xt/common/test/float_cmp.hh>
#include <dune/xt/common/fvector.hh>

using namespace Dune;
using XT::Common::create;
using XT::Common::FloatCmp::Style;

// add operator+= for std::array and std::vector
template <typename T, size_t N>
std::array<T, N>& operator+=(std::array<T, N>& arr, const std::array<T, N>& other)
{
  std::transform(arr.begin(), arr.end(), other.begin(), arr.begin(), std::plus<T>());
  return arr;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& vec, const std::vector<T>& other)
{
  assert(vec.size() == other.size());
  std::transform(vec.begin(), vec.end(), other.begin(), vec.begin(), std::plus<T>());
  return vec;
}

template <class V>
struct FloatCmpTest : public testing::Test
{
  typedef typename std::conditional<XT::Common::is_matrix<V>::value,
                                    typename XT::Common::MatrixAbstraction<V>::ScalarType,
                                    typename XT::Common::VectorAbstraction<V>::ScalarType>::type S;
  typedef typename std::conditional<XT::Common::is_matrix<V>::value,
                                    typename XT::Common::MatrixAbstraction<V>::RealType,
                                    typename XT::Common::VectorAbstraction<V>::RealType>::type R;
  static constexpr bool fieldtype_is_float = std::is_floating_point<R>::value;

  static const size_t s_size =
      XT::Common::is_matrix<V>::value
          ? (XT::Common::MatrixAbstraction<V>::has_static_size ? XT::Common::MatrixAbstraction<V>::static_rows
                                                               : VECSIZE)
          : (XT::Common::VectorAbstraction<V>::has_static_size ? XT::Common::VectorAbstraction<V>::static_size
                                                               : VECSIZE);
  static const size_t s_cols = XT::Common::is_matrix<V>::value ? (XT::Common::MatrixAbstraction<V>::has_static_size
                                                                      ? XT::Common::MatrixAbstraction<V>::static_cols
                                                                      : NUMCOLS)
                                                               : 1.;

  FloatCmpTest()
    : zero(create<V>(s_size, s_cols, create<S>(0, 0)))
    , one(create<V>(s_size, s_cols, create<S>(0, 1)))
    , neg_one(create<V>(s_size, s_cols, create<S>(0, -1)))
    , epsilon(create<V>(s_size, s_cols, XT::Common::FloatCmp::DEFAULT_EPSILON::value()))
    , eps_plus(create<V>(s_size, s_cols, XT::Common::FloatCmp::DEFAULT_EPSILON::value() * 1.1))
    , eps_minus(create<V>(s_size, s_cols, XT::Common::FloatCmp::DEFAULT_EPSILON::value() * 0.9))
    , two(create<V>(s_size, s_cols, create<S>(0, 2)))
    , test_config(DXTC_CONFIG.sub("test_common_float_cmp"))
  {
    one_plus_eps_minus_ = one;
    one_plus_eps_minus_ += eps_minus;
    one_plus_eps_plus_ = one;
    one_plus_eps_plus_ += eps_plus;
  }

  const V zero;
  const V one;
  const V neg_one;
  const V epsilon;
  const V eps_plus;
  const V eps_minus;
  const V two;
  V one_plus_eps_minus_;
  V one_plus_eps_plus_;
  const typename XT::Common::Configuration test_config;

  void check_eq()
  {
    TEST_DXTC_EXPECT_FLOAT_EQ(zero, zero);
    EXPECT_FALSE(FLOATCMP_EQ(zero, one));
    TEST_DXTC_EXPECT_FLOAT_EQ(one, one_plus_eps_minus_);
    EXPECT_FALSE(FLOATCMP_EQ(one, two));

    if (test_config["cmpstyle_is_relative"] == "true" && fieldtype_is_float)
      EXPECT_FALSE(FLOATCMP_EQ(zero, eps_minus));
    else
      TEST_DXTC_EXPECT_FLOAT_EQ(zero, eps_minus);

    if (test_config["cmpstyle_is_numpy"] == "true" || !fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_EQ(one, one_plus_eps_plus_);
    else
      EXPECT_FALSE(FLOATCMP_EQ(one, one_plus_eps_plus_));

    if (fieldtype_is_float)
      EXPECT_FALSE(FLOATCMP_EQ(zero, eps_plus));
    else
      TEST_DXTC_EXPECT_FLOAT_EQ(zero, eps_plus);
  }

  void check_ne()
  {
    EXPECT_FALSE(FLOATCMP_NE(zero, zero));
    TEST_DXTC_EXPECT_FLOAT_NE(zero, one);
    EXPECT_FALSE(FLOATCMP_NE(one, one_plus_eps_minus_));
    TEST_DXTC_EXPECT_FLOAT_NE(one, two);
    if (test_config["cmpstyle_is_relative"] == "true" && fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_NE(zero, eps_minus);
    else
      EXPECT_FALSE(FLOATCMP_NE(zero, eps_minus));

    if (test_config["cmpstyle_is_numpy"] == "true" || !fieldtype_is_float)
      EXPECT_FALSE(FLOATCMP_NE(one, one_plus_eps_plus_));
    else
      TEST_DXTC_EXPECT_FLOAT_NE(one, one_plus_eps_plus_);

    if (fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_NE(zero, eps_plus);
    else
      EXPECT_FALSE(FLOATCMP_NE(zero, eps_plus));
  }

  void check_gt()
  {
    EXPECT_FALSE(FLOATCMP_GT(zero, zero));
    TEST_DXTC_EXPECT_FLOAT_GT(one, zero);
    EXPECT_FALSE(FLOATCMP_GT(one_plus_eps_minus_, one));
    TEST_DXTC_EXPECT_FLOAT_GT(two, one);

    if (test_config["cmpstyle_is_relative"] == "true" && fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_GT(eps_minus, zero);
    else
      EXPECT_FALSE(FLOATCMP_GT(eps_minus, zero));

    if (test_config["cmpstyle_is_numpy"] == "true" || !fieldtype_is_float)
      EXPECT_FALSE(FLOATCMP_GT(one_plus_eps_plus_, one));
    else
      TEST_DXTC_EXPECT_FLOAT_GT(one_plus_eps_plus_, one);

    if (fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_GT(eps_plus, zero);
    else
      EXPECT_FALSE(FLOATCMP_GT(eps_plus, zero));
  }

  void check_lt()
  {
    EXPECT_FALSE(FLOATCMP_LT(zero, zero));
    TEST_DXTC_EXPECT_FLOAT_LT(zero, one);
    EXPECT_FALSE(FLOATCMP_LT(one, one_plus_eps_minus_));
    TEST_DXTC_EXPECT_FLOAT_LT(one, two);

    if (test_config["cmpstyle_is_relative"] == "true" && fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_LT(zero, eps_minus);
    else
      EXPECT_FALSE(FLOATCMP_LT(zero, eps_minus));

    if (test_config["cmpstyle_is_numpy"] == "true" || !fieldtype_is_float)
      EXPECT_FALSE(FLOATCMP_LT(one, one_plus_eps_plus_));
    else
      TEST_DXTC_EXPECT_FLOAT_LT(one, one_plus_eps_plus_);

    if (fieldtype_is_float)
      TEST_DXTC_EXPECT_FLOAT_LT(zero, eps_plus);
    else
      EXPECT_FALSE(FLOATCMP_LT(zero, eps_plus));
  }

  void check_ge()
  {
    TEST_DXTC_EXPECT_FLOAT_GE(zero, zero);
    TEST_DXTC_EXPECT_FLOAT_GE(eps_minus, zero);
    TEST_DXTC_EXPECT_FLOAT_GE(eps_plus, zero);
    TEST_DXTC_EXPECT_FLOAT_GE(one, zero);
    TEST_DXTC_EXPECT_FLOAT_GE(one_plus_eps_minus_, one);
    TEST_DXTC_EXPECT_FLOAT_GE(one_plus_eps_plus_, one);
    TEST_DXTC_EXPECT_FLOAT_GE(two, one);
  }

  void check_le()
  {
    TEST_DXTC_EXPECT_FLOAT_LE(zero, zero);
    TEST_DXTC_EXPECT_FLOAT_LE(zero, eps_minus);
    TEST_DXTC_EXPECT_FLOAT_LE(zero, eps_plus);
    TEST_DXTC_EXPECT_FLOAT_LE(zero, one);
    TEST_DXTC_EXPECT_FLOAT_LE(one, one_plus_eps_minus_);
    TEST_DXTC_EXPECT_FLOAT_LE(one, one_plus_eps_plus_);
    TEST_DXTC_EXPECT_FLOAT_LE(one, two);
  }

  void negative_numbers()
  {
    TEST_DXTC_EXPECT_FLOAT_LT(neg_one, one);
    TEST_DXTC_EXPECT_FLOAT_LT(neg_one, zero);
    EXPECT_FALSE(FLOATCMP_LT(neg_one, neg_one));
    TEST_DXTC_EXPECT_FLOAT_GT(one, neg_one);
    EXPECT_FALSE(FLOATCMP_GT(neg_one, neg_one));
    EXPECT_FALSE(FLOATCMP_LT(neg_one, neg_one));
    TEST_DXTC_EXPECT_FLOAT_NE(neg_one, two);
    EXPECT_FALSE(FLOATCMP_NE(neg_one, neg_one));
    EXPECT_FALSE(FLOATCMP_EQ(zero, neg_one));
    TEST_DXTC_EXPECT_FLOAT_EQ(neg_one, neg_one);
  }
}; // struct FloatCmpBase


TYPED_TEST_CASE(FloatCmpTest, TESTTYPES);
TYPED_TEST(FloatCmpTest, eq)
{
  this->check_eq();
}

TYPED_TEST(FloatCmpTest, ne)
{
  this->check_ne();
}

TYPED_TEST(FloatCmpTest, gt)
{
  this->check_gt();
}

TYPED_TEST(FloatCmpTest, lt)
{
  this->check_lt();
}

TYPED_TEST(FloatCmpTest, ge)
{
  this->check_ge();
}

TYPED_TEST(FloatCmpTest, le)
{
  this->check_le();
}

TYPED_TEST(FloatCmpTest, neg)
{
  if (std::is_signed<typename XT::Common::VectorAbstraction<TypeParam>::ScalarType>::value)
    this->negative_numbers();
}
