// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016 - 2017)
//   Rene Milk       (2012 - 2016, 2018)
//   Tobias Leibner  (2014, 2016)

#include <tuple>

#include <dune/xt/common/test/main.hxx>

#include <dune/common/dynmatrix.hh>
#include <dune/common/tupleutility.hh>
#include <dune/common/unused.hh>

#include <dune/xt/common/math.hh>
#include <dune/xt/common/ranges.hh>
#include <dune/xt/common/tuple.hh>

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

typedef std::tuple<A, B, B> MyTuple;

struct print_value
{
  template <class U, class V>
  static void run(std::ostream& out)
  {
    out << "(" << U::value << "," << V::value << ")" << std::endl;
  }

  template <class U, class V>
  static void run()
  {
    run<U, V>(std::cout);
  }
};

GTEST_TEST(TypeTransform, All)
{
  typedef TUPLE_TYPEDEFS_2_TUPLE(MyTuple, MyFancyType) MySubTupleType DUNE_UNUSED;
}

GTEST_TEST(Product, All)
{
  using namespace Dune::XT::Common::TupleProduct;
  typedef boost::mpl::vector<Int<1>, Int<2>> u_types;
  typedef boost::mpl::vector<Int<3>, Int<4>> v_types;
  typedef Combine<u_types, v_types, print_value>::Generate<> base_generator_type;
  base_generator_type::Run();
  base_generator_type::Run(std::cerr);
}
