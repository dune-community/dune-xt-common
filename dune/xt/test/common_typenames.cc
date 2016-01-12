// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014, 2016)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"

#include <iostream>

#include <dune/xt/common/type_utils.hh>
#include <dune/xt/common/validation.hh>

DUNE_XT_COMMON_TYPENAME(Dune::MPIHelper)

using namespace Dune::XT::Common;
using namespace std;

TEST(Typename, Knowns)
{
  EXPECT_EQ(Typename<unsigned long>::value(), "unsigned long");
  EXPECT_EQ(Typename<unsigned long>::value(), "unsigned long");
  EXPECT_EQ(Typename<int>::value(), "int");
  EXPECT_EQ(Typename<double>::value(), "double");
  EXPECT_EQ(Typename<float>::value(), "float");
  EXPECT_EQ(get_typename(1.f), "float");
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
  typedef ValidateAny<int> Valid;
  Valid v;
  EXPECT_EQ(get_typename(v), string("Parameter::ValidateAny<T>"));
}
