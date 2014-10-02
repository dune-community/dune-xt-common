// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <string>
#include <array>
#include <initializer_list>
#include <vector>
#include <dune/stuff/common/parallel/threadmanager.hh>
#include <dune/stuff/common/parallel/helper.hh>

using namespace Dune::Stuff;
using namespace Dune::Stuff::Common;

template <typename ThreadValue>
void check_eq(const ThreadValue& foo, const typename ThreadValue::ValueType& value)
{
  EXPECT_EQ(*foo, value);
  EXPECT_EQ(int(foo), value);
}

template <typename ThreadValue>
void check_eq(ThreadValue& foo, const typename ThreadValue::ValueType& value)
{
  auto& const_foo = static_cast<const ThreadValue&>(foo);
  check_eq(const_foo, value);
  const auto new_value = typename ThreadValue::ValueType(9);
  *foo = new_value;
  check_eq(const_foo, new_value);
}

typedef testing::Types<PerThreadValue<int>, TBBPerThreadValue<int>, FallbackPerThreadValue<int>> TLSTypes;

template <class T>
struct ThreadValueTest : public testing::Test
{
};

TYPED_TEST_CASE(ThreadValueTest, TLSTypes);
TYPED_TEST(ThreadValueTest, All)
{
  typedef TypeParam PTVType;
  typename PTVType::ValueType value(1);
  PTVType foo(value);
  check_eq(foo, value);
  foo = typename PTVType::ValueType(1);
  check_eq(foo, value);
  const auto new_value = *foo;
  const PTVType bar(*foo);
  check_eq(bar, new_value);
}

TEST(ThreadManagerTBB, All)
{
}
