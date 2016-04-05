// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Rene Milk       (2012 - 2016)

#include <dune/xt/common/test/main.hxx>

#include <dune/common/dynmatrix.hh>
#include <dune/common/tupleutility.hh>

#include <dune/xt/common/memory.hh>
#include <dune/xt/common/fvector.hh>

using namespace Dune::XT::Common;
typedef testing::Types<int> TestTypes;

template <class T>
struct ScopeTest : public testing::Test
{
  static constexpr T constant = 1;
  template <class P>
  void deref(P& p) {
    auto g = *p;
    EXPECT_EQ(constant, g);
  }

  void check_shared() {
    auto shared = std::make_shared<T>(constant);
    {
      ConstStorageProvider<T> shared_provider(shared);
      deref(shared);
    }
    // ok
    deref(shared);
  }
  void check_raw() {
    auto raw = new T(constant);
    {
      ConstStorageProvider<T> raw_provider(raw);
      deref(raw);
    }
    // fails
    deref(raw);
    EXPECT_NE(raw, nullptr);
  }
};
template <class T>
const T ScopeTest<T>::constant;

TYPED_TEST_CASE(ScopeTest, TestTypes);
TYPED_TEST(ScopeTest, All)
{
  this->check_shared();
  this->check_raw();
}
