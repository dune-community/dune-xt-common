// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012, 2014 - 2015)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"

#include <dune/common/dynmatrix.hh>
#include <dune/common/tupleutility.hh>

#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/common/ranges.hh>

using namespace Dune::Stuff::Common;
typedef testing::Types<double, int> TestTypes;
typedef testing::Types<std::complex<double>, double, int> ComplexTestTypes;

template <class T>
struct ClampTest : public testing::Test
{
  const T lower;
  const T upper;
  ClampTest()
    : lower(T(-1))
    , upper(T(1))
  {
  }
};

TYPED_TEST_CASE(ClampTest, TestTypes);
TYPED_TEST(ClampTest, All)
{
  EXPECT_EQ(clamp(TypeParam(-2), this->lower, this->upper), this->lower);
  EXPECT_EQ(clamp(TypeParam(2), this->lower, this->upper), this->upper);
  EXPECT_EQ(clamp(TypeParam(0), this->lower, this->upper), TypeParam(0));
}

template <class T>
struct EpsilonTest : public testing::Test
{
};

TYPED_TEST_CASE(EpsilonTest, TestTypes);
TYPED_TEST(EpsilonTest, All)
{
  EXPECT_NE(Epsilon<TypeParam>::value, TypeParam(0));
}

template <class T>
struct MinMaxAvgTest : public testing::Test
{
};

template <class MMType, class TypeParam>
void mmCheck(const MMType& mma)
{
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.min(), TypeParam(-4.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.max(), TypeParam(1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.average(), TypeParam(-1.0)));
}

TYPED_TEST_CASE(MinMaxAvgTest, TestTypes);
TYPED_TEST(MinMaxAvgTest, All)
{
  MinMaxAvg<TypeParam> mma;
  mma(-1);
  mma(1);
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.min(), TypeParam(-1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.max(), TypeParam(1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.average(), TypeParam(0.0)));
  mma(0);
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.min(), TypeParam(-1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.max(), TypeParam(1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.average(), TypeParam(0.0)));
  mma(-4);
  mmCheck<MinMaxAvg<TypeParam>, TypeParam>(mma);
  auto mmb = mma;
  mmCheck<MinMaxAvg<TypeParam>, TypeParam>(mmb);
}

TEST(OtherMath, Range)
{
  EXPECT_EQ((std::vector<unsigned int>{0, 1, 2, 3}), valueRange(4u));
  EXPECT_EQ((std::vector<int>{4, 3, 2, 1}), valueRange(4, 0, -1));
  EXPECT_EQ((std::vector<int>{-1, 0, 1}), valueRange(-1, 2));
  EXPECT_EQ((std::vector<float>()), valueRange(0.f));
  EXPECT_EQ((std::vector<float>{0.f}), valueRange(Epsilon<float>::value));
  Dune::FieldMatrix<double, 2, 2> fMatrix;
  fMatrix = 0.0;
  EXPECT_DOUBLE_EQ(fMatrix[1][1], 0.0);
  Dune::DynamicMatrix<double> dMatrix(2, 2);
  dMatrix = 0.0;
  EXPECT_DOUBLE_EQ(dMatrix[1][1], 0.0);
}

TEST(OtherMath, Sign)
{
  EXPECT_EQ(DSC::signum(1), 1);
  EXPECT_EQ(DSC::signum(-1), -1);
  EXPECT_EQ(DSC::signum(1.), 1);
  EXPECT_EQ(DSC::signum(-1.), -1);
}

TEST(OtherMath, AbsoluteValue)
{
  EXPECT_EQ(DSC::abs(1.0f), 1.0f);
  EXPECT_EQ(DSC::abs(-1l), 1l);
  EXPECT_EQ(DSC::abs(0u), 0u);
  EXPECT_EQ(DSC::abs(0), 0);
  EXPECT_EQ(DSC::abs(std::complex<int>(0)), 0);
  EXPECT_EQ(DSC::abs(std::complex<int>(-1)), 1);
}

TEST(OtherMath, FloatCmp)
{
  std::vector<double> ones{1., 1.};
  std::vector<double> twos{2., 2.};
  Dune::FieldVector<double, 2> dones(1.);
  EXPECT_TRUE(DSC::FloatCmp::eq(ones, ones));
  EXPECT_TRUE(DSC::FloatCmp::eq(dones, dones));
  EXPECT_FALSE(DSC::FloatCmp::ne(ones, ones));
  EXPECT_FALSE(DSC::FloatCmp::ne(dones, dones));

  Dune::FieldVector<double, 2> other(1.);
  other[1] = 0;
  EXPECT_TRUE(DSC::FloatCmp::ne(dones, other));
  other = 2;
  EXPECT_TRUE(DSC::FloatCmp::lt(dones, other));
  EXPECT_TRUE(DSC::FloatCmp::lt(ones, twos));
  EXPECT_TRUE(DSC::FloatCmp::gt(twos, ones));
}
