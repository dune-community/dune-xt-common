#include "test_common.hh"

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/logstreams.hh>
#include <boost/lexical_cast.hpp>
#include <vector>

using namespace Dune::Stuff::Common::String;
using namespace std;

TEST(StringTest, ConvertTo)
{
  EXPECT_EQ("9", convertTo<int>(convertFrom<int>("9")));
  EXPECT_EQ(double(0.1), convertFrom<double>(convertTo<double>(0.1)));
  EXPECT_EQ("0.10000000000000001",
            convertTo<double>(convertFrom<double>("0.1000000000000000055511151231257827021181583404541015625")));
  EXPECT_EQ("1", convertTo<bool>(convertFrom<bool>("1")));
  EXPECT_EQ("0", convertTo<bool>(convertFrom<bool>("0")));
  EXPECT_EQ("-1", convertTo<long>(convertFrom<long>("-1")));
}

TEST(StringTest, Hex)
{
  EXPECT_GT(boost::lexical_cast<HexTo<unsigned long>>(cout), 0u);
  EXPECT_EQ(boost::lexical_cast<HexTo<unsigned long>>("0x00000F"), 15u);
}

TEST(StringTest, ConvertFrom)
{
  EXPECT_EQ(9, convertFrom<int>("9"));
  EXPECT_EQ(0, convertFrom<int>("0"));
  EXPECT_EQ(true, convertFrom<bool>("1"));
  EXPECT_EQ(false, convertFrom<bool>("0"));
  EXPECT_THROW(convertFrom<int>(""), boost::bad_lexical_cast);
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
  string ts = fromTime(-1);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
