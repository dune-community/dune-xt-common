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

template <typename T>
void check_eq(const PerThreadValue<T>& foo, const T& value)
{
  EXPECT_EQ(*foo, value);
  EXPECT_EQ(int(foo), value);
}

template <typename T>
void check_eq(PerThreadValue<T>& foo, const T& value)
{
  check_eq(static_cast<const PerThreadValue<T>&>(foo), value);
  const auto new_value = T(9);
  *foo = new_value;
  check_eq(static_cast<const PerThreadValue<T>&>(foo), new_value);
}

TEST(ThreadValueTest, All)
{
  typedef int T;
  const T value(1);
  PerThreadValue<T> foo(value);
  check_eq(foo, value);
  const auto new_value        = *foo;
  const PerThreadValue<T> bar = foo;
  check_eq(bar, new_value);
  auto snafu = std::move(foo);
  check_eq(snafu, new_value);
}
