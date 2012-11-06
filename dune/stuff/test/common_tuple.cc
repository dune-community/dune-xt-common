#include "test_common.hh"

#include <dune/common/dynmatrix.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/common/tupleutility.hh>
#include <dune/stuff/common/tuple.hh>

class A
{
public:
  typedef int MyFancyType;
};

class B
{
public:
  typedef int MyFancyType;
};

typedef Dune::tuple<A, B, B> MyTuple;

TEST(TypeTransform, All)
{
  typedef TUPLE_TYPEDEFS_2_TUPLE(MyTuple, MyFancyType) MySubTupleType;
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
