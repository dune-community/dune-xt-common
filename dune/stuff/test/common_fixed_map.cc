// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.cxx"

#include <string>
#include <array>
#include <initializer_list>
#include <vector>
#include <dune/stuff/common/fixed_map.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/ranges.hh>

using namespace Dune::Stuff::Common;

TEST(FixedMapTest, All)
{
  const std::initializer_list<std::pair<std::string, int>> values{{"0", 0}, {"1", 1}, {"2", 2}};

  const FixedMap<std::string, int, 1> too_small(values);
  FixedMap<std::string, int, 3> fits(values);
  const FixedMap<std::string, int, 6> too_big(values);
  EXPECT_EQ(0, too_small[toString(0)]);
  EXPECT_THROW(too_small["1"], Dune::RangeError);
  for (int i : {0, 1, 2}) {
    EXPECT_EQ(i, too_big[toString(i)]);
  }
  for (int DUNE_UNUSED(i) : {3, 4, 5}) {
    EXPECT_EQ(int(), too_big[std::string()]);
  }
  auto size = fits.size();
  for (auto& pt : fits) {
    int value       = pt.second;
    std::string key = pt.first;
    EXPECT_EQ(key, toString(value));
    size--;
  }
  EXPECT_EQ(0, size);
  for (int i : {0, 1, 2}) {
    fits[toString(i)] += 1;
    EXPECT_EQ(i + 1, fits[toString(i)]);
  }

  EXPECT_EQ(std::make_pair(std::string("0"), 0), *too_big.begin());
  // death test segfaults inside gtest -> disabled
  //  EXPECT_DEATH(*too_big.end(), ".*");
}
