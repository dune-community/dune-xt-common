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

std::string print_vector(const std::vector<double>& vec)
{
  if (vec.empty())
    return "{}";
  else {
    std::stringstream ss;
    ss << "{" << std::setprecision(2) << std::scientific << vec[0];
    for (size_t ii = 1; ii < vec.size(); ++ii)
      ss << ", " << std::setprecision(2) << std::scientific << vec[ii];
    ss << "}";
    return ss.str();
  }
} // ... print_vector(...)

} // namespace internal

void check_eoc_study_for_success(const Common::ConvergenceStudy& /*study*/,
                                 const std::map<std::string, std::vector<double>>& /*results_map*/,
                                 const double& /*zero_tolerance*/)
{
  DUNE_THROW(NotImplemented, "This has to be migrated to match the new ConvergenceStudy!");
#if 0
  for (const auto& norm : study.used_norms()) {
    const auto expected_results = study.expected_results(norm);
    const auto results_search = results_map.find(norm);
    EXPECT_NE(results_search, results_map.end()) << "          norm = " << norm;
    if (results_search == results_map.end())
      return;
    const auto& actual_results = results_search->second;
    EXPECT_LE(actual_results.size(), expected_results.size()) << "          norm = " << norm;
    if (actual_results.size() > expected_results.size())
      return;
    for (size_t ii = 0; ii < actual_results.size(); ++ii) {
      if (!(expected_results[ii] < 0.0 || expected_results[ii] > 0.0)) {
        if (std::abs(actual_results[ii]) > zero_tolerance) {
          EXPECT_TRUE(false) << "Expected result is interpreted as zero and result is not close enough to zero!\n"
                             << "          norm: " << norm << "\n"
                             << "          zero_tolerance: " << zero_tolerance << "\n"
                             << "          actual_results[" << ii << "]   = " << actual_results[ii] << "\n"
                             << "          expected_results[" << ii << "] = " << expected_results[ii];
        }
      } else {
        const auto actual_result = internal::convert_to_scientific(actual_results[ii], 2);
        const auto expected_result = internal::convert_to_scientific(expected_results[ii], 2);
        const auto actual_exponent = actual_result.second;
        const auto expected_exponent = expected_result.second;
        EXPECT_EQ(expected_exponent, actual_exponent)
            << "          Exponent comparison (in scientific notation, precision 2) failed for\n"
            << "          norm: " << norm << "\n"
            << "          actual_results[" << ii << "]   = " << actual_results[ii] << "\n"
            << "          expected_results[" << ii << "] = " << expected_results[ii];
        if (actual_exponent != expected_exponent)
          return;
        const auto actual_coefficient = actual_result.first;
        const auto expected_coefficient = expected_result.first;
        EXPECT_EQ(expected_coefficient, actual_coefficient)
            << "          Coefficient comparison (in scientific notation, precision 2) failed for\n"
            << "          norm: " << norm << "\n"
            << "          actual_results[" << ii << "]   = " << actual_results[ii] << "\n"
            << "          expected_results[" << ii << "] = " << expected_results[ii];
      }
    }
  }
#endif // 0
} // ... check_eoc_study_for_success(...)

void print_collected_eoc_study_results(const std::map<std::string, std::vector<double>>& results, std::ostream& out)
{
  if (results.empty())
    DUNE_THROW(Common::Exceptions::wrong_input_given, "Given results must not be empty!");
  std::vector<std::string> actually_used_norms;
  for (const auto& element : results)
    actually_used_norms.push_back(element.first);

  out << "if (type == \"" << actually_used_norms[0] << "\")\n";
  out << "  return " << internal::print_vector(results.at(actually_used_norms[0])) << ";\n";
  for (size_t ii = 1; ii < actually_used_norms.size(); ++ii) {
    out << "else if (type == \"" << actually_used_norms[ii] << "\")\n";
    out << "  return " << internal::print_vector(results.at(actually_used_norms[ii])) << ";\n";
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
