#include "test_common.hh"

#include <dune/stuff/common/math.hh>
#include <dune/common/tupleutility.hh>

using namespace Dune::Stuff::Common;


template <class T>
struct MinMaxAvgTest
{
  static void run()
  {
    using namespace Math;
    MinMaxAvg<T> mma;
    mma(-1);
    mma(1);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(0.0)));
    mma(0);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(0.0)));
    mma(-4);
    MY_ASSERT(Dune::FloatCmp::eq(mma.min(), T(-4.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.max(), T(1.0)));
    MY_ASSERT(Dune::FloatCmp::eq(mma.average(), T(-1.0)));
  }
};

void vectorMathTest()
{
  typedef Dune::FieldVector<double, 2> Vector;
  typedef Dune::FieldMatrix<double, 2, 2> Matrix;
  {
    Vector a(0);
    a[0] = 1;
    Vector b(0);
    b[1] = 1;
    Matrix aa(0);
    aa[0][0] = 1;
    MY_ASSERT(Math::dyadicProduct<Matrix>(a, a) == aa);
    Matrix ab(0);
    ab[0][1] = 1;
    MY_ASSERT(Math::dyadicProduct<Matrix>(a, b) == ab);
  }
  {
    Matrix a(0);
    a[0][0] = 1;
    a[1][1] = 1;
    Matrix b(0);
    b[1][0] = 1;
    b[0][1] = 1;
    MY_ASSERT(Dune::FloatCmp::eq(Math::colonProduct(a, b), 0.0));
    MY_ASSERT(Dune::FloatCmp::eq(Math::colonProduct(a, a), 2.0));
  }
}

template <class T>
void clampTest()
{
  const T lower = T(-1);
  const T upper = T(1);
  MY_ASSERT(Math::clamp(T(-2), lower, upper) == lower);
  MY_ASSERT(Math::clamp(T(2), lower, upper) == upper);
  MY_ASSERT(Math::clamp(T(0), lower, upper) == T(0));
}

int main(int argc, char** argv)
{
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);
    MinMaxAvgTest<double>::run();
    MinMaxAvgTest<int>::run();
    vectorMathTest();
    clampTest<double>();
    clampTest<int>();
    //    Math::Epsilon<

  } catch (Dune::Exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}
