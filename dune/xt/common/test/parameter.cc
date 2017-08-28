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


GTEST_TEST(ParameterType, creation_and_report_and_ostreamout)
{
  std::map<ParameterType, std::string> expected_values = {
      {ParameterType(), "ParameterType({})"},
      {ParameterType("single_key"), "ParameterType({single_key: 1})"},
      {ParameterType("single_key_with_size_17", 17), "ParameterType({single_key_with_size_17: 17})"},
      {ParameterType({{"first_key_with_size_1", 1}, {"second_key_with_size_2", 2}}),
       "ParameterType({first_key_with_size_1: 1,\n               second_key_with_size_2: 2})"}};

  for (const auto& element : expected_values) {
    EXPECT_EQ(element.second, element.first.report());
    std::stringstream ss;
    ss << element.first;
    EXPECT_EQ(element.second, ss.str());
  }
}


GTEST_TEST(Parameter, creation_and_report_and_ostreamout)
{
  std::map<Parameter, std::string> expected_values = {
      {Parameter(), "Parameter({})"},
      {Parameter(1.), "Parameter({None: 1})"},
      {Parameter({1., 2.}), "Parameter({None: [1 2]})"},
      {Parameter("scalar_value", 17.), "Parameter({scalar_value: 17})"},
      {Parameter("vector_value", {17., 42.}), "Parameter({vector_value: [17 42]})"},
      {Parameter({{"first_scalar_value", {1.}}, {"second_vector_value", {1., 2.}}}),
       "Parameter({first_scalar_value: 1,\n           second_vector_value: [1 2]})"}};
  for (const auto& element : expected_values) {
    EXPECT_EQ(element.second, element.first.report());
    std::stringstream ss;
    ss << element.first;
    EXPECT_EQ(element.second, ss.str());
  }
}
