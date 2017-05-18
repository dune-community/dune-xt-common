// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)

#include <dune/xt/common/test/main.hxx>

#include <iostream>

#include <dune/xt/common/parameter.hh>

using namespace Dune::XT::Common;

GTEST_TEST(ParameterType, creation_and_report)
{
  for (const auto& param_type : {ParameterType(),
                                 ParameterType("single_key"),
                                 ParameterType("single_key_with_size_17", 17),
                                 ParameterType(std::make_pair("single_key_with_size_42", 42)),
                                 ParameterType({{"first_key_with_size_1", 1}, {"second_key_with_size_2", 2}})})
    std::cout << param_type << std::endl;
}

GTEST_TEST(Parameter, creation_and_report)
{
  for (const auto& mu : {Parameter(),
                         Parameter(1.),
                         Parameter("scalar_value", 17.),
                         Parameter("vector_value", {17., 42.}),
                         Parameter({{"first_scalar_value", {1.}}, {"second_vector_value", {1., 2.}}})})
    std::cout << mu << std::endl;
}
