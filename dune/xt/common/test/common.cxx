// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2018)
//   René Fritze     (2015 - 2018)
//   Tobias Leibner  (2016 - 2017)

#include "config.h"

#include <cmath>
#include <sys/time.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include <dune/xt/common/test/gtest/gtest.h>
#include <dune/xt/common/configuration.hh>
#include <dune/xt/common/exceptions.hh>

#include "common.hh"


void busywait(const size_t ms)
{
  // "round" up to next full 10 ms to align with native timer res
  const size_t milliseconds = (ms / 10) * 10 + 10;
  timeval start, end;
  gettimeofday(&start, NULL);
  do {
    gettimeofday(&end, NULL);
  } while (((end.tv_sec - start.tv_sec) * 1e6) + ((end.tv_usec - start.tv_usec)) < milliseconds * 1000);
} // ... busywait(...)


namespace Dune {
namespace XT {
namespace Common {
namespace Test {


std::string get_unique_test_name()
{
  const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
  DUNE_THROW_IF(!test_info, Exceptions::you_are_using_this_wrong, "You may only use this in the context of gtest!");
  static std::map<std::string, std::string> replacements{};
  auto replace_if = [&](const auto& id) {
    if (replacements.count(std::string(id)) > 0)
      return replacements.at(std::string(id));
    else
      return std::string(id);
  };
  std::string result;
  const auto* test_case_name = test_info->test_case_name();
  if (test_case_name) {
    result += replace_if(test_case_name);
  }
  const auto* name = test_info->name();
  if (name) {
    result += "." + replace_if(name);
  }
  const auto* type_param = test_info->type_param();
  if (type_param) {
    result += "." + replace_if(type_param);
  }
  const auto* value_param = test_info->value_param();
  if (value_param) {
    result += "." + replace_if(value_param);
  }
  return result;
}


} // namespace Test
} // namespace Common
namespace Test {
namespace internal {


std::pair<size_t, ssize_t> convert_to_scientific(const double number, const size_t precision)
{
  // see http://www.mathworks.com/matlabcentral/newsreader/view_thread/151859
  const double sign = (number > 0.0) ? 1.0 : -1.0;
  const double tmp = std::log10(std::abs(number));
  ssize_t exponent = ((tmp > 0) ? 1 : -1) * std::floor(std::abs(tmp));
  double coefficient = sign * std::pow(10.0, tmp - exponent);
  if (std::abs(coefficient) < 1.0) {
    coefficient *= 10.0;
    exponent -= 1;
  }
  const double factor = std::pow(10, precision);
  return std::make_pair(size_t(std::round(factor * coefficient)), exponent);
} // ... convert_to_scientific(...)


} // namespace internal


void check_eoc_study_for_success(
    const Common::Configuration& expected_results,
    const std::map<std::string, std::map<std::string, std::map<size_t, double>>>& actual_results,
    const double& zero_tolerance)
{
  auto get_keys = [](const auto& map) {
    std::vector<std::string> result;
    for (const auto& ele : map)
      result.push_back(Common::to_string(ele.first));
    return result;
  };
  EXPECT_GT(expected_results.getSubKeys().size(), 0)
      << "you have to provide at least one category!"
      << "\nThe actual results for instance contain the categories '" << get_keys(actual_results) << "'.";
  for (const auto& category : expected_results.getSubKeys()) {
    EXPECT_GT(actual_results.count(category), 0) << "missing data for category " << category << "!";
    const auto expected_category_data = expected_results.sub(category);
    const auto& actual_category_data = actual_results.at(category);
    EXPECT_GT(expected_category_data.getValueKeys().size(), 0)
        << "you have to provide at least one type for category " << category
        << "!\nThe actual_results for instance contain the types '" << get_keys(actual_category_data)
        << "' for this category.";
    for (const auto& type : expected_category_data.getValueKeys()) {
      EXPECT_GT(actual_category_data.count(type), 0)
          << "missing data for category " << category << " and type " << type << "!";
      const auto expected_values = expected_category_data.template get<std::vector<double>>(type);
      const auto& actual_type_data = actual_category_data.at(type);
      for (size_t level = 0; level < expected_values.size(); ++level) {
        EXPECT_GT(actual_type_data.count(level), 0)
            << "missing data for category " << category << ", type " << type << " and level " << level << "!";
        const auto& actual_value = actual_type_data.at(level);
        const auto& expected_value = expected_values[level];
        if (!(expected_value < 0.0 || expected_value > 0.0)) {
          if (std::abs(actual_value) > zero_tolerance) {
            EXPECT_TRUE(false)
                << "          Expected result is interpreted as zero and result is not close enough to zero!\n"
                << "          level:    " << level << "\n"
                << "          category: " << category << "\n"
                << "          type:     " << type << "\n"
                << "          zero_tolerance: " << zero_tolerance << "\n"
                << "          actual_value   = " << actual_value << "\n"
                << "          expected_value = " << expected_value;
          }
        } else {
          const auto actual_result = internal::convert_to_scientific(actual_value, 2);
          const auto expected_result = internal::convert_to_scientific(expected_value, 2);
          const auto actual_exponent = actual_result.second;
          const auto expected_exponent = expected_result.second;
          EXPECT_EQ(expected_exponent, actual_exponent)
              << "          Exponent comparison (in scientific notation, precision 2) failed for\n"
              << "          level:    " << level << "\n"
              << "          category: " << category << "\n"
              << "          type:     " << type << "\n"
              << "          actual_value   = " << actual_value << "\n"
              << "          expected_value = " << expected_value;
          if (actual_exponent != expected_exponent)
            return;
          const auto actual_coefficient = actual_result.first;
          const auto expected_coefficient = expected_result.first;
          EXPECT_EQ(expected_coefficient, actual_coefficient)
              << "          Coefficient comparison (in scientific notation, precision 2) failed for\n"
              << "          level:    " << level << "\n"
              << "          category: " << category << "\n"
              << "          type:     " << type << "\n"
              << "          actual_value   = " << actual_value << "\n"
              << "          expected_value = " << expected_value;
        }
      }
    }
  }
} // ... check_eoc_study_for_success(...)


void print_collected_eoc_study_results(const std::map<std::string, std::vector<double>>& results, std::ostream& out)
{
  if (results.empty())
    DUNE_THROW(Common::Exceptions::wrong_input_given, "Given results must not be empty!");
  std::vector<std::string> actually_used_norms;
  for (const auto& element : results)
    actually_used_norms.push_back(element.first);
  out << "if (type == \"" << actually_used_norms[0] << "\")\n";
  out << "  return " << results.at(actually_used_norms[0]) << ";\n";
  for (size_t ii = 1; ii < actually_used_norms.size(); ++ii) {
    out << "else if (type == \"" << actually_used_norms[ii] << "\")\n";
    out << "  return " << results.at(actually_used_norms[ii]) << ";\n";
  }
  out << "else\n";
  out << "  EXPECT_TRUE(false) << \"test results missing for type: \" << type;\n";
  out << "return {};" << std::endl;
} // ... print_collected_eoc_study_results(...)


unsigned int grid_elements()
{
  return DXTC_CONFIG.has_key("test.gridelements") // <- doing this so complicated to
             ? DXTC_CONFIG.get<unsigned int>(
                   "test.gridelements", 3u, Common::ValidateLess<unsigned int>(2u)) //    silence the WARNING: ...
             : 3u;
} // ... grid_elements(...)


} // namespace Test
} // namespace XT
} // namespace Dune
