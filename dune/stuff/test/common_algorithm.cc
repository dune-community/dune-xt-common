// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2015)

#include "main.hxx"

#include <vector>
#include <dune/stuff/common/algorithm.hh>

using namespace Dune::Stuff::Common;

struct Moveable
{
  Moveable(int i)
    : v(i)
  {
  }
  int v;
};

TEST(MoveIfTest, All)
{
  using namespace std;
  typedef vector<unique_ptr<Moveable>> Vec;
  typedef Vec::value_type Ptr;
  const auto size = 6;
  Vec values;
  for (int i : valueRange(size)) {
    values.emplace_back(new Moveable(i * 2));
  }
  Vec out;
  const auto half_size = size / 2;
  move_if(values.begin(), values.end(), std::back_inserter(out), [](Ptr& m) { return m->v < size; });
  EXPECT_EQ(out.size(), half_size);
  auto ctr = [](Ptr& ptr) { return ptr != nullptr; };
  EXPECT_EQ(std::count_if(values.begin(), values.end(), ctr), half_size);
  for (auto i : valueRange(half_size)) {
    EXPECT_NE(out[i], nullptr);
    EXPECT_EQ(i * 2, out[i]->v);
  }
  for (auto i : valueRange(half_size)) {
    EXPECT_EQ(nullptr, values[i]);
    EXPECT_EQ((i + half_size) * 2, values[i + half_size]->v);
  }
}
