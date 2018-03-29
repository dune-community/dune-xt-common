// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)

#include <dune/xt/common/test/main.hxx>

#include <dune/xt/common/matrix.hh>
#include <dune/xt/common/fmatrix.hh>

using MatrixTypes = ::testing::Types<std::tuple<Dune::FieldMatrix<int, 1, 1>, Int<1>, Int<1>>,
                                     std::tuple<Dune::FieldMatrix<int, 2, 3>, Int<2>, Int<3>>,
                                     std::tuple<Dune::FieldMatrix<int, 3, 1>, Int<3>, Int<1>>,
                                     std::tuple<Dune::XT::Common::FieldMatrix<int, 1, 1>, Int<1>, Int<1>>,
                                     std::tuple<Dune::XT::Common::FieldMatrix<int, 2, 3>, Int<2>, Int<3>>,
                                     std::tuple<Dune::XT::Common::FieldMatrix<int, 3, 1>, Int<3>, Int<1>>,
                                     std::tuple<Dune::DynamicMatrix<int>, Int<1>, Int<1>>,
                                     std::tuple<Dune::DynamicMatrix<int>, Int<2>, Int<3>>,
                                     std::tuple<Dune::DynamicMatrix<int>, Int<3>, Int<1>>>;


template <class Tuple>
struct SerializeRowwiseTest : public ::testing::Test
{
  using MatrixType = typename std::tuple_element<0, Tuple>::type;
  using M = Dune::XT::Common::MatrixAbstraction<MatrixType>;
  using ScalarType = typename M::ScalarType;
  static const constexpr size_t rows = std::tuple_element<1, Tuple>::type::value;
  static const constexpr size_t cols = std::tuple_element<2, Tuple>::type::value;

  SerializeRowwiseTest()
    : matrix_(M::create(rows, cols))
  {
    for (size_t ii = 0; ii < rows; ++ii)
      for (size_t jj = 0; jj < cols; ++jj)
        M::set_entry(matrix_, ii, jj, static_cast<ScalarType>(100 * ii + jj));
  }

  template <class T>
  void check_type()
  {
    std::unique_ptr<T[]> data_ptr = Dune::XT::Common::serialize_rowwise<T>(matrix_);
    size_t cc = 0;
    for (size_t ii = 0; ii < rows; ++ii)
      for (size_t jj = 0; jj < cols; ++jj)
        EXPECT_EQ(M::get_entry(matrix_, ii, jj), data_ptr[cc++]);
  }

  void check_default()
  {
    auto data_ptr = Dune::XT::Common::serialize_rowwise(matrix_);
    size_t cc = 0;
    for (size_t ii = 0; ii < rows; ++ii)
      for (size_t jj = 0; jj < cols; ++jj)
        EXPECT_EQ(M::get_entry(matrix_, ii, jj), data_ptr[cc++]);
  }

  MatrixType matrix_;
}; // struct SerializeRowwiseTest


TYPED_TEST_CASE(SerializeRowwiseTest, MatrixTypes);
TYPED_TEST(SerializeRowwiseTest, default)
{
  this->check_default();
}
TYPED_TEST(SerializeRowwiseTest, int)
{
  this->template check_type<int>();
}
TYPED_TEST(SerializeRowwiseTest, long_int)
{
  this->template check_type<long int>();
}
TYPED_TEST(SerializeRowwiseTest, long_long_int)
{
  this->template check_type<long long int>();
}
TYPED_TEST(SerializeRowwiseTest, ssize_t)
{
  this->template check_type<ssize_t>();
}
TYPED_TEST(SerializeRowwiseTest, unsigned_int)
{
  this->template check_type<unsigned int>();
}
TYPED_TEST(SerializeRowwiseTest, unsigned_long_int)
{
  this->template check_type<unsigned long int>();
}
TYPED_TEST(SerializeRowwiseTest, unsigned_long_long_int)
{
  this->template check_type<unsigned long long int>();
}
TYPED_TEST(SerializeRowwiseTest, size_t)
{
  this->template check_type<size_t>();
}
TYPED_TEST(SerializeRowwiseTest, float)
{
  this->template check_type<float>();
}
TYPED_TEST(SerializeRowwiseTest, double)
{
  this->template check_type<double>();
}
