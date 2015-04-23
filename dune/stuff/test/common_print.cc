// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <dune/stuff/common/print.hh>

using namespace DSC;
using namespace std;

TEST(OutputIterator, All)
{
  const vector<int> ints{0, 1, 2};
  const string s_ints("0,1,2");
  const vector<string> strings{"a", "b", "c"};
  const string s_strings("a,b,c");
  stringstream stream;
  std::copy(ints.begin(), ints.end(), PrefixOutputIterator<int>(stream, ","));
  EXPECT_EQ(stream.str(), s_ints);
  stream.str("");
  std::copy(strings.begin(), strings.end(), PrefixOutputIterator<string>(stream, ","));
  EXPECT_EQ(stream.str(), s_strings);
}
