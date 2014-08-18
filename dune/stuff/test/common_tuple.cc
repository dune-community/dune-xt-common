// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/stuff/common/reenable_warnings.hh>
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

struct print_value
{
  template <class U, class V>
  static void run()
  {
    std::cout << "(" << U::value << "," << V::value << ")" << std::endl;
  }
};

TEST(TypeTransform, All)
{
  typedef TUPLE_TYPEDEFS_2_TUPLE(MyTuple, MyFancyType) MySubTupleType;
}

TEST(Product, All)
{
  using namespace DSC::TupleProduct;
  typedef boost::mpl::vector<Int<1>, Int<2>> u_types;
  typedef boost::mpl::vector<Int<3>, Int<4>> v_types;
  typedef Combine<u_types, v_types, print_value>::Generate<> base_generator_type;
  base_generator_type::Run();
}
