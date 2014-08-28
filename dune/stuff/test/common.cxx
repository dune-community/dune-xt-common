// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

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


void check_for_success(const Dune::Stuff::Common::ConvergenceStudy& study,
                       const std::map<std::string, std::vector<double>>& errors_map)
{
  for (const auto& norm : study.used_norms()) {
    const auto expected_results = study.expected_results(norm);
    const auto errors_search = errors_map.find(norm);
    EXPECT_NE(errors_search, errors_map.end()) << "          norm = " << norm;
    const auto& errors = errors_search->second;
    EXPECT_LE(errors.size(), expected_results.size()) << "          norm = " << norm;
    for (size_t ii = 0; ii < errors.size(); ++ii)
      EXPECT_LE(errors[ii], expected_results[ii]) << "          norm = " << norm << ", level = " << ii;
  }
} // ... check_for_success(...)


} // namespace Test
} // namespace Stuff
} // namespace Dune
