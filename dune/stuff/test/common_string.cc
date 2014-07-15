// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/logstreams.hh>
#include <boost/lexical_cast.hpp>
#include <bits/functexcept.h>
#include <vector>

using namespace Dune::Stuff::Common;
using namespace std;

TEST(StringTest, ConvertTo)
{
  EXPECT_EQ("9", toString(fromString<int>("9")));
  EXPECT_EQ("P", toString(fromString<char>("P")));
  EXPECT_EQ(double(0.1), fromString<double>(toString<double>(0.1)));
  EXPECT_EQ("0.10000000000000001",
            toString(fromString<double>("0.1000000000000000055511151231257827021181583404541015625")));
  EXPECT_EQ("1", toString(fromString<bool>("1")));
  EXPECT_EQ("0", toString(fromString<bool>("0")));
  EXPECT_EQ("-1", toString(fromString<long>("-1")));
}

TEST(StringTest, Hex)
{
  EXPECT_GT(boost::lexical_cast<HexToString<unsigned long>>(cout), 0u);
  EXPECT_EQ(boost::lexical_cast<HexToString<unsigned long>>("0x00000F"), 15u);
}

TEST(StringTest, ConvertFrom)
{
  EXPECT_EQ(9, fromString<int>("9"));
  EXPECT_EQ(0, fromString<int>("0"));
  EXPECT_EQ('p', fromString<char>(toString<char>('p')));
  EXPECT_EQ(-1, fromString<char>(toString<char>(-1)));
  EXPECT_EQ(true, fromString<bool>("1"));
  EXPECT_EQ(false, fromString<bool>("0"));
  EXPECT_THROW(fromString<int>(""), std::invalid_argument);
}

TEST(StringTest, Whitespace)
{
  EXPECT_EQ("---------", whitespaceify("\t\t\t\t\t\t\t\t\t", '-'));
}

TEST(StringTest, Tokenizer)
{
  const string seps(" \t;");
  const string msg("a t\tkk;;g");
  const vector<string> tokens_default    = {"a", "t", "kk", "", "g"};
  const vector<string> tokens_compressed = {"a", "t", "kk", "g"};
  EXPECT_EQ(tokens_default, tokenize(msg, seps, boost::algorithm::token_compress_off));
  EXPECT_EQ(tokens_compressed, tokenize(msg, seps, boost::algorithm::token_compress_on));
  const string num_msg("-1 2;;4");
  vector<int> numbers_default    = {-1, 2, 0, 4};
  vector<int> numbers_compressed = {-1, 2, 4};
  EXPECT_EQ(numbers_default, tokenize<int>(num_msg, seps, boost::algorithm::token_compress_off));
  EXPECT_EQ(numbers_compressed, tokenize<int>(num_msg, seps, boost::algorithm::token_compress_on));
}

TEST(StringTest, TimeString)
{
  string ts = stringFromTime(-1);
}

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}
