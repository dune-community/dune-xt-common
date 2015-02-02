// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#define DUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS 1

#include "main.hxx"

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/vector.hh>
#include <dune/stuff/la/container/common.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/istl.hh>

using namespace Dune;
using Stuff::Common::create;
using Stuff::Common::FloatCmp::Style::numpy;
using Stuff::Common::FloatCmp::Style::relativeWeak;
using Stuff::Common::FloatCmp::Style::relativeStrong;
using Stuff::Common::FloatCmp::Style::absolute;
using Stuff::Common::FloatCmp::Style::defaultStyle;

static const unsigned int vec_size = 4;


struct FloatCmpBase : public testing::Test
{
  template <class Z, class E, class O>
  static void check_eq(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<numpy>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<relativeWeak>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<relativeStrong>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<absolute>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<defaultStyle>(zero, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::eq(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<numpy>(zero, 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<absolute>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_FALSE(Stuff::Common::FloatCmp::eq(zero, 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<numpy>(zero, 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<absolute>(zero, 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_FALSE(Stuff::Common::FloatCmp::eq(zero, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<numpy>(zero, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeWeak>(zero, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeStrong>(zero, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<absolute>(zero, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<defaultStyle>(zero, one));

    EXPECT_TRUE(Stuff::Common::FloatCmp::eq(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<numpy>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<absolute>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::eq(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<numpy>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<absolute>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::eq<defaultStyle>(one, one + 1.1 * epsilon)); /* <- NOTE */

    EXPECT_FALSE(Stuff::Common::FloatCmp::eq(one, 2 * one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<numpy>(one, 2 * one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeWeak>(one, 2 * one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<relativeStrong>(one, 2 * one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<absolute>(one, 2 * one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::eq<defaultStyle>(one, 2 * one));
  } // ... check_eq(...)

  template <class Z, class E, class O>
  static void check_ne(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<numpy>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<relativeWeak>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<relativeStrong>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<absolute>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<defaultStyle>(zero, zero));

    EXPECT_FALSE(Stuff::Common::FloatCmp::ne(zero, 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<absolute>(zero, 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ne(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<absolute>(zero, 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ne(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<numpy>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeWeak>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeStrong>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<absolute>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<defaultStyle>(zero, one));

    EXPECT_FALSE(Stuff::Common::FloatCmp::ne(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<numpy>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<absolute>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_FALSE(Stuff::Common::FloatCmp::ne(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<numpy>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<absolute>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::ne<defaultStyle>(one, one + 1.1 * epsilon)); /* <- NOTE */

    EXPECT_TRUE(Stuff::Common::FloatCmp::ne(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<numpy>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeWeak>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<relativeStrong>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<absolute>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ne<defaultStyle>(one, 2 * one));
  } // ... check_ne(...)

  template <class Z, class E, class O>
  static void check_gt(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<numpy>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<relativeWeak>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<relativeStrong>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<absolute>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<defaultStyle>(zero, zero));

    EXPECT_FALSE(Stuff::Common::FloatCmp::gt(0.9 * epsilon, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<numpy>(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeWeak>(0.9 * epsilon, zero)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeStrong>(0.9 * epsilon, zero)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<absolute>(0.9 * epsilon, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<defaultStyle>(0.9 * epsilon, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::gt(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<numpy>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeWeak>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeStrong>(1.1 * epsilon, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<absolute>(1.1 * epsilon, zero)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<defaultStyle>(1.1 * epsilon, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::gt(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<numpy>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeWeak>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeStrong>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<absolute>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<defaultStyle>(one, zero));

    EXPECT_FALSE(Stuff::Common::FloatCmp::gt(one + 0.9 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<numpy>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<relativeWeak>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<relativeStrong>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<absolute>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<defaultStyle>(one + 0.9 * epsilon, one));

    EXPECT_FALSE(Stuff::Common::FloatCmp::gt(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<numpy>(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeWeak>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeStrong>(one + 1.1 * epsilon, one));
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<absolute>(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::gt<defaultStyle>(one + 1.1 * epsilon, one)); /* <- NOTE */

    EXPECT_TRUE(Stuff::Common::FloatCmp::gt(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<numpy>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeWeak>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<relativeStrong>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<absolute>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::gt<defaultStyle>(2 * one, one));
  } // ... check_gt(...)

  template <class Z, class E, class O>
  static void check_lt(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<numpy>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<relativeWeak>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<relativeStrong>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<absolute>(zero, zero));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<defaultStyle>(zero, zero));

    EXPECT_FALSE(Stuff::Common::FloatCmp::lt(zero, 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<absolute>(zero, 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::lt(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<absolute>(zero, 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::lt(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<numpy>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeWeak>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeStrong>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<absolute>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<defaultStyle>(zero, one));

    EXPECT_FALSE(Stuff::Common::FloatCmp::lt(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<numpy>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<absolute>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_FALSE(Stuff::Common::FloatCmp::lt(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<numpy>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<absolute>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(Stuff::Common::FloatCmp::lt<defaultStyle>(one, one + 1.1 * epsilon)); /* <- NOTE */

    EXPECT_TRUE(Stuff::Common::FloatCmp::lt(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<numpy>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeWeak>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<relativeStrong>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<absolute>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::lt<defaultStyle>(one, 2 * one));
  } // ... check_lt(...)

  template <class Z, class E, class O>
  static void check_ge(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(zero, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(0.9 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(0.9 * epsilon, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(1.1 * epsilon, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(1.1 * epsilon, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(one, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(one, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(one + 0.9 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(one + 0.9 * epsilon, one));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(one + 1.1 * epsilon, one));

    EXPECT_TRUE(Stuff::Common::FloatCmp::ge(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<numpy>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeWeak>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<relativeStrong>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<absolute>(2 * one, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::ge<defaultStyle>(2 * one, one));
  } // ... check_ge(...)

  template <class Z, class E, class O>
  static void check_le(const Z& zero, const E& epsilon, const O& one)
  {
    EXPECT_TRUE(Stuff::Common::FloatCmp::le(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(zero, zero));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(zero, zero));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(zero, 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(zero, 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(zero, one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(zero, one));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(one, one + 1.1 * epsilon));

    EXPECT_TRUE(Stuff::Common::FloatCmp::le(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<numpy>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeWeak>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<relativeStrong>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<absolute>(one, 2 * one));
    EXPECT_TRUE(Stuff::Common::FloatCmp::le<defaultStyle>(one, 2 * one));
  } // ... check_le(...)
}; // struct FloatCmpBase


template <class S>
struct FloatCmpScalar : public FloatCmpBase
{
  static void check_eq()
  {
    FloatCmpBase::check_eq(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }

  static void check_ne()
  {
    FloatCmpBase::check_ne(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }

  static void check_gt()
  {
    FloatCmpBase::check_gt(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }

  static void check_lt()
  {
    FloatCmpBase::check_lt(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }

  static void check_ge()
  {
    FloatCmpBase::check_ge(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }

  static void check_le()
  {
    FloatCmpBase::check_le(S(0), S(Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()), S(1));
  }
}; // struct FloatCmpScalar

template <class V>
struct FloatCmpVector : public FloatCmpBase
{
  typedef typename DSC::VectorAbstraction<V>::ScalarType S;

  static void check_eq()
  {
    FloatCmpBase::check_eq(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }

  static void check_ne()
  {
    FloatCmpBase::check_ne(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }

  static void check_gt()
  {
    FloatCmpBase::check_gt(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }

  static void check_lt()
  {
    FloatCmpBase::check_lt(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }

  static void check_ge()
  {
    FloatCmpBase::check_ge(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }

  static void check_le()
  {
    FloatCmpBase::check_le(create<V>(vec_size, 0),
                           create<V>(vec_size, Stuff::Common::FloatCmp::DefaultEpsilon<S>::value()),
                           create<V>(vec_size, 1));
  }
}; // struct FloatCmpVector


typedef testing::Types<double
                       //                      , long double // <- this requires a patch in dune/common/float_cmp.cc
                       //                      (bc. of std::max and 1e-6)
                       > ScalarTypes;

TYPED_TEST_CASE(FloatCmpScalar, ScalarTypes);
TYPED_TEST(FloatCmpScalar, eq)
{
  this->check_eq();
}
TYPED_TEST(FloatCmpScalar, ne)
{
  this->check_ne();
}
TYPED_TEST(FloatCmpScalar, gt)
{
  this->check_gt();
}
TYPED_TEST(FloatCmpScalar, lt)
{
  this->check_lt();
}
TYPED_TEST(FloatCmpScalar, ge)
{
  this->check_ge();
}
TYPED_TEST(FloatCmpScalar, le)
{
  this->check_le();
}


typedef testing::Types<std::vector<double>, Dune::FieldVector<double, vec_size>,
                       Dune::Stuff::Common::FieldVector<double, vec_size>, Dune::DynamicVector<double>,
                       Dune::Stuff::LA::CommonDenseVector<double>
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlDenseVector<double>
#endif
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenDenseVector<double>, Dune::Stuff::LA::EigenMappedDenseVector<double>
#endif
                       > VectorTypes;

TYPED_TEST_CASE(FloatCmpVector, VectorTypes);
TYPED_TEST(FloatCmpVector, eq)
{
  this->check_eq();
}
TYPED_TEST(FloatCmpVector, ne)
{
  this->check_ne();
}
TYPED_TEST(FloatCmpVector, gt)
{
  this->check_gt();
}
TYPED_TEST(FloatCmpVector, lt)
{
  this->check_lt();
}
TYPED_TEST(FloatCmpVector, ge)
{
  this->check_ge();
}
TYPED_TEST(FloatCmpVector, le)
{
  this->check_le();
}


#if !HAVE_DUNE_ISTL

TEST(DISABLED_IstlDenseVector_FloatCmpVector, eq)
{
}
TEST(DISABLED_IstlDenseVector_FloatCmpVector, ne)
{
}
TEST(DISABLED_IstlDenseVector_FloatCmpVector, gt)
{
}
TEST(DISABLED_IstlDenseVector_FloatCmpVector, lt)
{
}
TEST(DISABLED_IstlDenseVector_FloatCmpVector, ge)
{
}
TEST(DISABLED_IstlDenseVector_FloatCmpVector, le)
{
}

#endif
#if !HAVE_EIGEN

TEST(DISABLED_EigenDenseVector_FloatCmpVector, eq)
{
}
TEST(DISABLED_EigenDenseVector_FloatCmpVector, ne)
{
}
TEST(DISABLED_EigenDenseVector_FloatCmpVector, gt)
{
}
TEST(DISABLED_EigenDenseVector_FloatCmpVector, lt)
{
}
TEST(DISABLED_EigenDenseVector_FloatCmpVector, ge)
{
}
TEST(DISABLED_EigenDenseVector_FloatCmpVector, le)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, eq)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, ne)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, gt)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, lt)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, ge)
{
}
TEST(DISABLED_EigenMappedDenseVector_FloatCmpVector, le)
{
}

#endif
