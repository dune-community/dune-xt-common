// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Rene Milk       (2017)

#include <dune/xt/common/test/main.hxx>

#include <iostream>

#include <dune/xt/common/type_traits.hh>

using namespace Dune::XT::Common;
using namespace std;

GTEST_TEST(VectorAbstraction, Transfer)
{
  {
    using Vec = std::vector<int>;
    using Abstract = VectorAbstraction<Vec>;
    using Transferred = Abstract::Transfer<std::string>;
  }
  {
    using Vec = std::array<int, 2>;
    using Abstract = VectorAbstraction<Vec>;
    using Transferred = Abstract::Transfer<std::string>;
  }
}
