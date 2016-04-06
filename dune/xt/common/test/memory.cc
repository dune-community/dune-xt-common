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


struct ScopeTest : public testing::Test
{
  typedef int T;
  static constexpr T constant = 1;
  template <class P>
  void deref(P& p) {
    EXPECT_NE(p, nullptr);
    auto g = *p;
    EXPECT_EQ(constant, g);
  }

  template <template <class F> class Provider, class P>
  void scope(P& p) {
    Provider<T> shared_provider(p);
    deref(p);
  }

  void check_shared() {
    auto shared = std::make_shared<T>(constant);
    scope<ConstStorageProvider>(shared);
    deref(shared);
    scope<StorageProvider>(shared);
    deref(shared);
  }
};
constexpr typename ScopeTest::T ScopeTest::constant;

TEST_F(ScopeTest, All)
{
  this->check_shared();
}
