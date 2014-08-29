// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include <cmath>
#include <sys/time.h>

#include <dune/stuff/test/gtest/gtest.h>

#include "common.hh"

void busywait(const int ms)
{
  // "round" up to next full 10 ms to align with native timer res
  const int milliseconds = (ms / 10) * 10 + 10;
  timeval start, end;
  gettimeofday(&start, NULL);
  do {
    gettimeofday(&end, NULL);
  } while (((end.tv_sec - start.tv_sec) * 1e6) + ((end.tv_usec - start.tv_usec)) < milliseconds * 1000);
} // ... busywait(...)


namespace Dune {
namespace Stuff {
namespace Test {
namespace internal {


std::pair<size_t, ssize_t> convert_to_scientific(const double number, const size_t precision)
{
  // see http://www.mathworks.com/matlabcentral/newsreader/view_thread/151859
  const double sign  = (number > 0.0) ? 1.0 : -1.0;
  const double tmp   = std::log10(std::abs(number));
  ssize_t exponent   = ((tmp > 0) ? 1 : -1) * std::floor(std::abs(tmp));
  double coefficient = sign * std::pow(10.0, tmp - exponent);
  if (std::abs(coefficient) < 1.0) {
    coefficient *= 10.0;
    exponent -= 1;
  }
  const double factor = std::pow(10, precision);
  //  std::cout << "number      = " << number << std::endl;
  //  std::cout << "exponent    = " << exponent << std::endl;
  //  std::cout << "coefficient = " << coefficient << std::endl;
  //  std::cout << "scaled         = " << std::round(factor * coefficient) << std::endl;
  //  std::cout << "size_t(scaled) = " << size_t(std::round(factor * coefficient)) << std::endl << std::endl;

  return std::make_pair(size_t(std::round(factor * coefficient)), exponent);
} // ... convert_to_scientific(...)


} // namespace internal


void check_for_success(const Dune::Stuff::Common::ConvergenceStudy& study,
                       const std::map<std::string, std::vector<double>>& results_map)
{
  for (const auto& norm : study.used_norms()) {
    const auto expected_results = study.expected_results(norm);
    const auto results_search = results_map.find(norm);
    EXPECT_NE(results_search, results_map.end()) << "          norm = " << norm;
    const auto& actual_results = results_search->second;
    EXPECT_LE(actual_results.size(), expected_results.size()) << "          norm = " << norm;
    for (size_t ii = 0; ii < actual_results.size(); ++ii) {
      const auto actual_result     = internal::convert_to_scientific(actual_results[ii], 2);
      const auto expected_result   = internal::convert_to_scientific(expected_results[ii], 2);
      const auto actual_exponent   = actual_result.second;
      const auto expected_exponent = expected_result.second;
      EXPECT_EQ(actual_exponent, expected_exponent)
          << "          Exponent comparison (in scientific notation, precision 2) failed for\n"
          << "          norm: " << norm << "\n"
          << "          actual_results[" << ii << "]   = " << actual_results[ii] << "\n"
          << "          expected_results[" << ii << "] = " << expected_results[ii];
      const auto actual_coefficient   = actual_result.first;
      const auto expected_coefficient = expected_result.first;
      EXPECT_EQ(actual_coefficient, expected_coefficient)
          << "          Coefficient comparison (in scientific notation, precision 2) failed for\n"
          << "          norm: " << norm << "\n"
          << "          actual_results[" << ii << "]   = " << actual_results[ii] << "\n"
          << "          expected_results[" << ii << "] = " << expected_results[ii];
    }
  }
} // ... check_for_success(...)


} // namespace Test
} // namespace Stuff
} // namespace Dune
