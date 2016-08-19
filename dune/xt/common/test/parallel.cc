// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016)
//   Rene Milk       (2014 - 2015)

#include <dune/xt/common/test/main.hxx>

#include <string>
#include <memory>
#include <array>
#include <initializer_list>
#include <vector>
#include <dune/xt/common/parallel/threadmanager.hh>
#include <dune/xt/common/parallel/threadstorage.hh>
#include <dune/xt/common/parallel/helper.hh>

using namespace Dune::XT;
using namespace Dune::XT::Common;

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

    const auto new_value = typename ThreadValue::ValueType(9);
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

typedef testing::Types<FallbackPerThreadValue<int>, PerThreadValue<int>,
#if HAVE_TBB
                       TBBPerThreadValue<int>, TBBPerThreadValue<const int>,
#endif
                       FallbackPerThreadValue<const int>, PerThreadValue<const int>>
    TLSTypes;

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

TEST(ThreadManager, All)
{
  auto& tm = Dune::XT::Common::threadManager();
  EXPECT_LE(tm.current_threads(), tm.max_threads());
  EXPECT_LT(tm.thread(), tm.current_threads());
}
