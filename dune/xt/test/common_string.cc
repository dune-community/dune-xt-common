// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2015)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"

#include <vector>

#include <dune/common/fmatrix.hh>
#include <dune/common/densematrix.hh>
#include <dune/common/fvector.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/densevector.hh>

#include <dune/xt/common/string.hh>
#include <dune/xt/common/logstreams.hh>
#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/fvector.hh>
#include <dune/xt/common/fmatrix.hh>
#include <dune/xt/la/container/common.hh>
#include <dune/xt/la/container/eigen.hh>
#include <dune/xt/la/container/istl.hh>

using namespace Dune::XT::Common;
using namespace std;

// define types and expected values for the typed tests
template <class MatrixType>
struct MatrixStringTestDouble : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2; 3 4]", toString(fromString<MatrixType>("[1.0 2; 3.0 4]")));
    EXPECT_THROW(fromString<MatrixType>("[1 2; 3 4]", 3, 3), Exceptions::conversion_error);
  }
};

template <class MatrixType>
struct MatrixStringTestChar : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2; 3 4]", toString(fromString<MatrixType>("[1 2; 3 4]")));
    EXPECT_THROW(fromString<MatrixType>("[1 2; 3 4]", 3, 3), Exceptions::conversion_error);
  }
};

template <class VectorType>
struct VectorStringTestDouble : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2 3]", toString(fromString<VectorType>("[1.0 2 3.0]")));
    EXPECT_THROW(fromString<VectorType>("[1.0 2 3.0]", 4), Exceptions::conversion_error);
  }
};

template <class VectorType>
struct VectorStringTestInt : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2 3]", toString(fromString<VectorType>("[1 2 3]")));
    EXPECT_THROW(fromString<VectorType>("[1 2 3]", 4), Exceptions::conversion_error);
  }
};

typedef testing::Types<std::vector<double>, Dune::XT::LA::CommonDenseVector<double>, Dune::DynamicVector<double>,
                       Dune::FieldVector<double, 3>, FieldVector<double, 3>
#if HAVE_EIGEN
                       ,
                       Dune::XT::LA::EigenDenseVector<double>, Dune::XT::LA::EigenMappedDenseVector<double>
#endif
#if HAVE_DUNE_ISTL
                       ,
                       Dune::XT::LA::IstlDenseVector<double>
#endif
                       > VectorTypesDouble;

typedef testing::Types<std::vector<int>, Dune::XT::LA::CommonDenseVector<int>, Dune::DynamicVector<int>,
                       Dune::FieldVector<int, 3>, FieldVector<int, 3>
#if HAVE_DUNE_ISTL
                       ,
                       Dune::XT::LA::IstlDenseVector<int>
#endif
                       > VectorTypesInt;

typedef testing::Types<Dune::DynamicMatrix<double>, Dune::XT::LA::CommonDenseMatrix<double>, FieldMatrix<double, 2, 2>,
                       Dune::FieldMatrix<double, 2, 2>
#if HAVE_EIGEN
                       ,
                       Dune::XT::LA::EigenDenseMatrix<double>
#endif
                       > MatrixTypesDouble;

typedef testing::Types<Dune::DynamicMatrix<char>, FieldMatrix<char, 2, 2>, Dune::FieldMatrix<char, 2, 2>>
    MatrixTypesChar;

// fromString/toString tests for vector and matrix types
TYPED_TEST_CASE(MatrixStringTestDouble, MatrixTypesDouble);
TYPED_TEST(MatrixStringTestDouble, CheckDouble)
{
  this->check();
}

TYPED_TEST_CASE(MatrixStringTestChar, MatrixTypesChar);
TYPED_TEST(MatrixStringTestChar, CheckChar)
{
  this->check();
}

TYPED_TEST_CASE(VectorStringTestDouble, VectorTypesDouble);
TYPED_TEST(VectorStringTestDouble, CheckDouble)
{
  this->check();
}

TYPED_TEST_CASE(VectorStringTestInt, VectorTypesInt);
TYPED_TEST(VectorStringTestInt, CheckInt)
{
  this->check();
}

// Additional fromString/toString tests
TEST(StringTest, ConvertTo)
{
  EXPECT_EQ("9", toString(fromString<int>("9")));
  EXPECT_EQ("P", toString(fromString<char>("P")));
  EXPECT_EQ(double(0.1), fromString<double>(toString<double>(0.1)));
  EXPECT_EQ("0.100006", toString(fromString<double>("0.1000055511151231257827021181583404541015625")));
  EXPECT_EQ(1e-14, fromString<double>("1e-14"));
  EXPECT_EQ(1e-14, fromString<double>("1E-14"));
  EXPECT_EQ("1", toString(fromString<bool>("1")));
  EXPECT_EQ("0", toString(fromString<bool>("0")));

  EXPECT_EQ("-1", toString(fromString<long>("-1")));
  typedef std::complex<double> Complex;
  EXPECT_EQ("-1.3+0i", toString(fromString<Complex>("-1.3")));
  EXPECT_EQ("-1.3+1i", toString(fromString<Complex>("-1.3+1i")));
  EXPECT_EQ("1.3-1i", toString(fromString<Complex>("1.3-1i")));
  EXPECT_EQ("0-1i", toString(fromString<Complex>("0-1i")));
}

TEST(StringTest, ConvertFrom)
{
  EXPECT_EQ(9, fromString<int>("9"));
  EXPECT_EQ(0, fromString<int>("0"));
  EXPECT_EQ('p', fromString<char>(toString('p')));
  EXPECT_EQ(-1, fromString<char>(toString(char(-1))));
  EXPECT_THROW(fromString<char>("sd"), Exceptions::conversion_error);
  EXPECT_EQ(true, fromString<bool>("1"));
  EXPECT_EQ(true, fromString<bool>("true"));
  EXPECT_EQ(true, fromString<bool>("True"));
  EXPECT_EQ(false, fromString<bool>("0"));
  EXPECT_EQ(false, fromString<bool>("false"));
  EXPECT_EQ(false, fromString<bool>("False"));
  EXPECT_THROW(fromString<int>(""), std::invalid_argument);
  typedef std::complex<double> Complex;
  EXPECT_EQ(Complex(1, -1), fromString<Complex>("+1-1i"));
  EXPECT_EQ(Complex(1, 0), fromString<Complex>("+1"));
  EXPECT_EQ(Complex(0, -1), fromString<Complex>("0-1i"));
}

// Hex, whitespacify, tokenize, stringFromTime tests
TEST(StringTest, Hex)
{
  EXPECT_GT(boost::lexical_cast<HexToString<unsigned long>>(&cout), 0u);
  EXPECT_EQ(boost::lexical_cast<HexToString<unsigned long>>("0x00000F"), 15u);
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
