#include "test_common.hh"

#include <dune/stuff/common/math.hh>
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
  EXPECT_EQ(Math::dyadicProduct<Matrix>(this->a, this->a), this->aa);
  EXPECT_EQ(Math::dyadicProduct<Matrix>(this->a, this->b), this->ab);
}
TYPED_TEST(VectorMath, colonProduct)
{
  EXPECT_TRUE(Dune::FloatCmp::eq(Math::colonProduct(this->c, this->d), 0.0));
  EXPECT_TRUE(Dune::FloatCmp::eq(Math::colonProduct(this->c, this->c), 2.0));
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
  EXPECT_EQ(Math::clamp(TypeParam(-2), this->lower, this->upper), this->lower);
  EXPECT_EQ(Math::clamp(TypeParam(2), this->lower, this->upper), this->upper);
  EXPECT_EQ(Math::clamp(TypeParam(0), this->lower, this->upper), TypeParam(0));
}

template <class T>
struct EpsilonTest : public testing::Test
{
};

TYPED_TEST_CASE(EpsilonTest, TestTypes);
TYPED_TEST(EpsilonTest, All)
{
  EXPECT_NE(Math::Epsilon<TypeParam>::value, TypeParam(0));
}

template <class T>
struct MinMaxAvgTest : public testing::Test
{
};

TYPED_TEST_CASE(MinMaxAvgTest, TestTypes);
TYPED_TEST(MinMaxAvgTest, All)
{
  using namespace Math;
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
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.min(), TypeParam(-4.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.max(), TypeParam(1.0)));
  EXPECT_TRUE(Dune::FloatCmp::eq(mma.average(), TypeParam(-1.0)));
}

TEST(OtherMath, Range)
{
  EXPECT_EQ((std::vector<unsigned int>{0, 1, 2, 3}), Math::range(4u));
  EXPECT_EQ((std::vector<int>{4, 3, 2, 1}), Math::range(4, 0, -1));
  EXPECT_EQ((std::vector<int>{-1, 0, 1}), Math::range(-1, 2));
  EXPECT_EQ((std::vector<float>()), Math::range(0.f));
  EXPECT_EQ((std::vector<float>{0.f}), Math::range(Math::Epsilon<float>::value));
}

TEST(OtherMath, Sign)
{
  EXPECT_TRUE(false); // tests missing
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
