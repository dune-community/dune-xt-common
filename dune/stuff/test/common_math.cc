// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/common/tupleutility.hh>

using namespace Dune::Stuff::Common;
typedef testing::Types<double, int> TestTypes;

template <class T>
struct VectorMath : public testing::Test
{
  typedef Dune::FieldVector<double, 2> Vector;
  typedef Dune::FieldMatrix<double, 2, 2> Matrix;
  Vector a, b;
  Matrix c, d, aa, ab;

  VectorMath()
    : a(0)
    , b(0)
    , c(0)
    , d(0)
    , aa(0)
    , ab(0)
  {
    a[0]     = 1;
    b[1]     = 1;
    aa[0][0] = 1;
    ab[0][1] = 1;
    c[0][0]  = 1;
    c[1][1]  = 1;
    d[1][0]  = 1;
    d[0][1]  = 1;
  }
};

TYPED_TEST_CASE(VectorMath, TestTypes);
TYPED_TEST(VectorMath, dyadicProduct)
{
  typedef typename VectorMath<TypeParam>::Matrix Matrix;
  EXPECT_EQ(dyadicProduct<Matrix>(this->a, this->a), this->aa);
  EXPECT_EQ(dyadicProduct<Matrix>(this->a, this->b), this->ab);
}
TYPED_TEST(VectorMath, colonProduct)
{
  EXPECT_TRUE(Dune::FloatCmp::eq(colonProduct(this->c, this->d), 0.0));
  EXPECT_TRUE(Dune::FloatCmp::eq(colonProduct(this->c, this->c), 2.0));
}

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
  EXPECT_EQ(DSC::sign(1), 1);
  EXPECT_EQ(DSC::sign(-1), -1);
  EXPECT_EQ(DSC::sign(1.), 1);
  EXPECT_EQ(DSC::sign(-1.), -1);
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

  EXPECT_TRUE(DSC::FloatCmp::vec_ne(dones, Dune::FieldVector<double, 2>{1, 0}));
  EXPECT_TRUE(DSC::FloatCmp::lt(dones, Dune::FieldVector<double, 2>{2, 2}));
  EXPECT_TRUE(DSC::FloatCmp::lt(ones, twos));
  EXPECT_TRUE(DSC::FloatCmp::gt(twos, ones));
}

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}
