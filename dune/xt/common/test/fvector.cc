// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Rene Milk       (2018)

#include <dune/xt/common/test/main.hxx> // <- Needs to come first, include the config.h.

#include <dune/xt/common/fvector.hh>


// using this function to allow for {...} syntax below (would not work in the EXPECT_EQ macro directly).
template <class A, int a, class B, int b>
void check(const Dune::XT::Common::FieldVector<A, a>& expected_result, const Dune::FieldVector<B, b>& actual_result)
{
  EXPECT_EQ(expected_result, actual_result);
}


GTEST_TEST(hstack, dune_field_vector)
{
  const Dune::FieldVector<double, 1> one(1);
  const Dune::FieldVector<double, 2> two(2);
  // hstack is specialized for one and two arguments, so testing up to three should be enough
  using Dune::XT::Common::hstack;
  check(Dune::XT::Common::FieldVector<double, 2>({1, 3}), hstack(one, 3));
  check(Dune::XT::Common::FieldVector<double, 2>({3, 1}), hstack(3, one));
  check(Dune::XT::Common::FieldVector<double, 3>({1, 2, 2}), hstack(one, two));
  check(Dune::XT::Common::FieldVector<double, 3>({2, 2, 1}), hstack(two, one));
  check(Dune::XT::Common::FieldVector<double, 4>({3, 1, 2, 2}), hstack(3, one, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 3, 2, 2}), hstack(one, 3, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 2, 2, 3}), hstack(one, two, 3));
}


GTEST_TEST(hstack, dune_xt_common_field_vector)
{
  using namespace Dune;
  const Dune::XT::Common::FieldVector<double, 1> one(1);
  const Dune::XT::Common::FieldVector<double, 2> two(2);
  // hstack is specialized for one and two arguments, so testing up to three should be enough
  using Dune::XT::Common::hstack;
  check(Dune::XT::Common::FieldVector<double, 2>({1, 3}), hstack(one, 3));
  check(Dune::XT::Common::FieldVector<double, 2>({3, 1}), hstack(3, one));
  check(Dune::XT::Common::FieldVector<double, 3>({1, 2, 2}), hstack(one, two));
  check(Dune::XT::Common::FieldVector<double, 3>({2, 2, 1}), hstack(two, one));
  check(Dune::XT::Common::FieldVector<double, 4>({3, 1, 2, 2}), hstack(3, one, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 3, 2, 2}), hstack(one, 3, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 2, 2, 3}), hstack(one, two, 3));
}


GTEST_TEST(hstack, both_field_vectors)
{
  using namespace Dune;
  const Dune::XT::Common::FieldVector<double, 1> one(1);
  const Dune::FieldVector<double, 2> two(2);
  // hstack is specialized for one and two arguments, so testing up to three should be enough
  using Dune::XT::Common::hstack;
  check(Dune::XT::Common::FieldVector<double, 3>({1, 2, 2}), hstack(one, two));
  check(Dune::XT::Common::FieldVector<double, 3>({2, 2, 1}), hstack(two, one));
  check(Dune::XT::Common::FieldVector<double, 4>({3, 1, 2, 2}), hstack(3, one, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 3, 2, 2}), hstack(one, 3, two));
  check(Dune::XT::Common::FieldVector<double, 4>({1, 2, 2, 3}), hstack(one, two, 3));
}
