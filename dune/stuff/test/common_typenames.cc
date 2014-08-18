// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <iostream>

#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/validation.hh>

STUFF_TYPENAME(Dune::MPIHelper)

using namespace Dune::Stuff::Common;
using namespace std;

TEST(Typename, Knowns)
{
  EXPECT_EQ(Typename<unsigned long>::value(), "unsigned long");
  EXPECT_EQ(Typename<unsigned long>::value(), "unsigned long");
  EXPECT_EQ(Typename<int>::value(), "int");
  EXPECT_EQ(Typename<double>::value(), "double");
  EXPECT_EQ(Typename<float>::value(), "float");
  EXPECT_EQ(getTypename(1.f), "float");
  EXPECT_EQ(Typename<long>::value(), "long");
  EXPECT_EQ(Typename<unsigned int>::value(), "unsigned int");
  EXPECT_EQ(Typename<unsigned long>::value(), "unsigned long");
  EXPECT_EQ(Typename<char>::value(), "char");
}

TEST(Typename, Unknowns)
{
  EXPECT_NE(Typename<Dune::Exception>::value(), string());
}

TEST(Typename, Extended)
{
  EXPECT_EQ(Typename<Dune::MPIHelper>::value(), string("Dune::MPIHelper"));
  {
    using namespace Dune;
    EXPECT_EQ(Typename<MPIHelper>::value(), string("Dune::MPIHelper"));
  }
  typedef Dune::Stuff::Common::ValidateAny<int> Valid;
  Valid v;
  EXPECT_EQ(getTypename(v), string("Dune::Stuff::Common::Parameter::ValidateAny<T>"));
}
