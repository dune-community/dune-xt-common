// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <string>
#include <memory>
#include <array>
#include <initializer_list>
#include <vector>
#include <dune/stuff/common/parallel/threadmanager.hh>
#include <dune/stuff/common/parallel/threadstorage.hh>
#include <dune/stuff/common/parallel/helper.hh>

using namespace Dune::Stuff;
using namespace Dune::Stuff::Common;

template <typename ThreadValue>
void value_check(ThreadValue& foo, const typename ThreadValue::ValueType& value)
{
  EXPECT_EQ(*foo, value);
  EXPECT_EQ(int(foo), value);
  EXPECT_EQ(*(foo.operator->()), value);
}

template <typename ThreadValue,
          bool = std::is_const<typename ThreadValue::ValueType>::value || std::is_const<ThreadValue>::value>
struct Checker
{
  static void check_eq(ThreadValue& foo, const typename ThreadValue::ValueType& value)
  {
    auto& const_foo = static_cast<const ThreadValue&>(foo);
    value_check(const_foo, value);
    EXPECT_GT(const_foo.sum(), 0);
  }
};

template <typename ThreadValue>
struct Checker<ThreadValue, false /*valuetype is not const*/>
{
  static void check_eq(ThreadValue& foo, const typename ThreadValue::ValueType& value)
  {
    auto& const_foo = static_cast<const ThreadValue&>(foo);
    value_check(const_foo, value);
    EXPECT_GT(const_foo.sum(), 0);

    const auto new_value                 = typename ThreadValue::ValueType(9);
    typename ThreadValue::ValueType& bar = *foo;
    bar = new_value;
    value_check(const_foo, new_value);
  }
};

template <typename ThreadValue>
void check_eq(ThreadValue& foo, const typename ThreadValue::ValueType& value)
{
  Checker<ThreadValue>::check_eq(foo, value);
}

// typedef testing::Types< PerThreadValue<int>, TBBPerThreadValue<int>, FallbackPerThreadValue<int>> TLSTypes;
typedef testing::Types<TBBPerThreadValue<const int>, FallbackPerThreadValue<const int>, PerThreadValue<const int>,
                       TBBPerThreadValue<int>, FallbackPerThreadValue<int>, PerThreadValue<int>> TLSTypes;

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
