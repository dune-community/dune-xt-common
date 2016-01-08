// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015)
//   Rene Milk       (2015)

#define DUNE_XT_COMMON_TEST_MAIN_CATCH_EXCEPTIONS 1

#include "main.hxx"

#include <vector>

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#include <dune/xt/common/float_cmp.hh>
#include <dune/xt/test/float_cmp.hh>
#include <dune/xt/common/fvector.hh>
#include <dune/xt/la/container/common.hh>
#include <dune/xt/la/container/eigen.hh>
#include <dune/xt/la/container/istl.hh>

using namespace Dune;
using XT::Common::create;
using namespace XT::Common::FloatCmp;
using XT::Common::VectorAbstraction;
static const Style numpy          = Style::numpy;
static const Style relativeWeak   = Style::relativeWeak;
static const Style relativeStrong = Style::relativeStrong;
static const Style absolute       = Style::absolute;
static const Style defaultStyle   = Style::defaultStyle;

static const size_t vec_size = 4;

template <typename V, size_t s_size>
struct FloatCmpBase : public testing::Test
{

  typedef typename VectorAbstraction<V>::ScalarType S;

  FloatCmpBase()
    : zero(create<V>(s_size, create<S>(0, 0)))
    , one(create<V>(s_size, create<S>(0, 1)))
    , epsilon(create<V>(s_size, DefaultEpsilon<S>::value()))
  {
  }

  const V zero;
  const V one;
  const V epsilon;

  void check_eq()
  {
    DXTC_EXPECT_FLOAT_EQ(zero, zero);
    DXTC_EXPECT_FLOAT_EQ<numpy>(zero, zero);
    DXTC_EXPECT_FLOAT_EQ<relativeWeak>(zero, zero);
    DXTC_EXPECT_FLOAT_EQ<relativeStrong>(zero, zero);
    DXTC_EXPECT_FLOAT_EQ<absolute>(zero, zero);
    DXTC_EXPECT_FLOAT_EQ<defaultStyle>(zero, zero);

    DXTC_EXPECT_FLOAT_EQ(zero, 0.9 * epsilon);
    DXTC_EXPECT_FLOAT_EQ<numpy>(zero, 0.9 * epsilon);
    EXPECT_FALSE(eq<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(eq<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    DXTC_EXPECT_FLOAT_EQ<absolute>(zero, 0.9 * epsilon);
    DXTC_EXPECT_FLOAT_EQ<defaultStyle>(zero, 0.9 * epsilon);

    EXPECT_FALSE(eq(zero, 1.1 * epsilon));
    EXPECT_FALSE(eq<numpy>(zero, 1.1 * epsilon));
    EXPECT_FALSE(eq<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_FALSE(eq<relativeStrong>(zero, 1.1 * epsilon));
    DXTC_EXPECT_FLOAT_EQ<absolute>(zero, 1.1 * epsilon); /* <- NOTE */
    EXPECT_FALSE(eq<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_FALSE(eq(zero, one));
    EXPECT_FALSE(eq<numpy>(zero, one));
    EXPECT_FALSE(eq<relativeWeak>(zero, one));
    EXPECT_FALSE(eq<relativeStrong>(zero, one));
    EXPECT_FALSE(eq<absolute>(zero, one));
    EXPECT_FALSE(eq<defaultStyle>(zero, one));

    const auto eps_plus = one + 0.9 * epsilon;
    DXTC_EXPECT_FLOAT_EQ(one, eps_plus);
    DXTC_EXPECT_FLOAT_EQ<numpy>(one, eps_plus);
    DXTC_EXPECT_FLOAT_EQ<relativeWeak>(one, eps_plus);
    DXTC_EXPECT_FLOAT_EQ<relativeStrong>(one, eps_plus);
    DXTC_EXPECT_FLOAT_EQ<absolute>(one, eps_plus);
    DXTC_EXPECT_FLOAT_EQ<defaultStyle>(one, eps_plus);

    DXTC_EXPECT_FLOAT_EQ(one, one + 1.1 * epsilon); /* <- NOTE */
    DXTC_EXPECT_FLOAT_EQ<numpy>(one, one + 1.1 * epsilon); /* <- NOTE */
    EXPECT_FALSE(eq<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(eq<relativeStrong>(one, one + 1.1 * epsilon));
    DXTC_EXPECT_FLOAT_EQ<absolute>(one, one + 1.1 * epsilon); /* <- NOTE */
    DXTC_EXPECT_FLOAT_EQ<defaultStyle>(one, one + 1.1 * epsilon); /* <- NOTE */

    EXPECT_FALSE(eq(one, 2. * one));
    EXPECT_FALSE(eq<numpy>(one, 2. * one));
    EXPECT_FALSE(eq<relativeWeak>(one, 2. * one));
    EXPECT_FALSE(eq<relativeStrong>(one, 2. * one));
    EXPECT_FALSE(eq<absolute>(one, 2. * one));
    EXPECT_FALSE(eq<defaultStyle>(one, 2. * one));
  }

  void check_ne()
  {
    EXPECT_FALSE(ne(zero, zero));
    EXPECT_FALSE(ne<numpy>(zero, zero));
    EXPECT_FALSE(ne<relativeWeak>(zero, zero));
    EXPECT_FALSE(ne<relativeStrong>(zero, zero));
    EXPECT_FALSE(ne<absolute>(zero, zero));
    EXPECT_FALSE(ne<defaultStyle>(zero, zero));

    EXPECT_FALSE(ne(zero, 0.9 * epsilon));
    EXPECT_FALSE(ne<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(ne<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(ne<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(ne<absolute>(zero, 0.9 * epsilon));
    EXPECT_FALSE(ne<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(ne(zero, 1.1 * epsilon));
    EXPECT_TRUE(ne<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(ne<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(ne<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_FALSE(ne<absolute>(zero, 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(ne<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(ne(zero, one));
    EXPECT_TRUE(ne<numpy>(zero, one));
    EXPECT_TRUE(ne<relativeWeak>(zero, one));
    EXPECT_TRUE(ne<relativeStrong>(zero, one));
    EXPECT_TRUE(ne<absolute>(zero, one));
    EXPECT_TRUE(ne<defaultStyle>(zero, one));

    EXPECT_FALSE(ne(one, one + 0.9 * epsilon));
    EXPECT_FALSE(ne<numpy>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(ne<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(ne<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(ne<absolute>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(ne<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_FALSE(ne(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(ne<numpy>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(ne<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(ne<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(ne<absolute>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(ne<defaultStyle>(one, one + 1.1 * epsilon)); /* <- NOTE */

    EXPECT_TRUE(ne(one, 2. * one));
    EXPECT_TRUE(ne<numpy>(one, 2. * one));
    EXPECT_TRUE(ne<relativeWeak>(one, 2. * one));
    EXPECT_TRUE(ne<relativeStrong>(one, 2. * one));
    EXPECT_TRUE(ne<absolute>(one, 2. * one));
    EXPECT_TRUE(ne<defaultStyle>(one, 2. * one));
  }

  void check_gt()
  {
    EXPECT_FALSE(gt(zero, zero));
    EXPECT_FALSE(gt<numpy>(zero, zero));
    EXPECT_FALSE(gt<relativeWeak>(zero, zero));
    EXPECT_FALSE(gt<relativeStrong>(zero, zero));
    EXPECT_FALSE(gt<absolute>(zero, zero));
    EXPECT_FALSE(gt<defaultStyle>(zero, zero));

    EXPECT_FALSE(gt(0.9 * epsilon, zero));
    EXPECT_FALSE(gt<numpy>(0.9 * epsilon, zero));
    EXPECT_TRUE(gt<relativeWeak>(0.9 * epsilon, zero)); /* <- NOTE */
    EXPECT_TRUE(gt<relativeStrong>(0.9 * epsilon, zero)); /* <- NOTE */
    EXPECT_FALSE(gt<absolute>(0.9 * epsilon, zero));
    EXPECT_FALSE(gt<defaultStyle>(0.9 * epsilon, zero));

    EXPECT_TRUE(gt(1.1 * epsilon, zero));
    EXPECT_TRUE(gt<numpy>(1.1 * epsilon, zero));
    EXPECT_TRUE(gt<relativeWeak>(1.1 * epsilon, zero));
    EXPECT_TRUE(gt<relativeStrong>(1.1 * epsilon, zero));
    EXPECT_FALSE(gt<absolute>(1.1 * epsilon, zero)); /* <- NOTE */
    EXPECT_TRUE(gt<defaultStyle>(1.1 * epsilon, zero));

    EXPECT_TRUE(gt(one, zero));
    EXPECT_TRUE(gt<numpy>(one, zero));
    EXPECT_TRUE(gt<relativeWeak>(one, zero));
    EXPECT_TRUE(gt<relativeStrong>(one, zero));
    EXPECT_TRUE(gt<absolute>(one, zero));
    EXPECT_TRUE(gt<defaultStyle>(one, zero));

    EXPECT_FALSE(gt(one + 0.9 * epsilon, one));
    EXPECT_FALSE(gt<numpy>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(gt<relativeWeak>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(gt<relativeStrong>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(gt<absolute>(one + 0.9 * epsilon, one));
    EXPECT_FALSE(gt<defaultStyle>(one + 0.9 * epsilon, one));

    EXPECT_FALSE(gt(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_FALSE(gt<numpy>(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_TRUE(gt<relativeWeak>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(gt<relativeStrong>(one + 1.1 * epsilon, one));
    EXPECT_FALSE(gt<absolute>(one + 1.1 * epsilon, one)); /* <- NOTE */
    EXPECT_FALSE(gt<defaultStyle>(one + 1.1 * epsilon, one)); /* <- NOTE */

    EXPECT_TRUE(gt(2. * one, one));
    EXPECT_TRUE(gt<numpy>(2. * one, one));
    EXPECT_TRUE(gt<relativeWeak>(2. * one, one));
    EXPECT_TRUE(gt<relativeStrong>(2. * one, one));
    EXPECT_TRUE(gt<absolute>(2. * one, one));
    EXPECT_TRUE(gt<defaultStyle>(2. * one, one));
  }

  void check_lt()
  {
    EXPECT_FALSE(lt(zero, zero));
    EXPECT_FALSE(lt<numpy>(zero, zero));
    EXPECT_FALSE(lt<relativeWeak>(zero, zero));
    EXPECT_FALSE(lt<relativeStrong>(zero, zero));
    EXPECT_FALSE(lt<absolute>(zero, zero));
    EXPECT_FALSE(lt<defaultStyle>(zero, zero));

    EXPECT_FALSE(lt(zero, 0.9 * epsilon));
    EXPECT_FALSE(lt<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(lt<relativeWeak>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(lt<relativeStrong>(zero, 0.9 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(lt<absolute>(zero, 0.9 * epsilon));
    EXPECT_FALSE(lt<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(lt(zero, 1.1 * epsilon));
    EXPECT_TRUE(lt<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(lt<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(lt<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_FALSE(lt<absolute>(zero, 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(lt<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(lt(zero, one));
    EXPECT_TRUE(lt<numpy>(zero, one));
    EXPECT_TRUE(lt<relativeWeak>(zero, one));
    EXPECT_TRUE(lt<relativeStrong>(zero, one));
    EXPECT_TRUE(lt<absolute>(zero, one));
    EXPECT_TRUE(lt<defaultStyle>(zero, one));

    EXPECT_FALSE(lt(one, one + 0.9 * epsilon));
    EXPECT_FALSE(lt<numpy>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(lt<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(lt<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(lt<absolute>(one, one + 0.9 * epsilon));
    EXPECT_FALSE(lt<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_FALSE(lt(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(lt<numpy>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_TRUE(lt<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(lt<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_FALSE(lt<absolute>(one, one + 1.1 * epsilon)); /* <- NOTE */
    EXPECT_FALSE(lt<defaultStyle>(one, one + 1.1 * epsilon)); /* <- NOTE */

    EXPECT_TRUE(lt(one, 2. * one));
    EXPECT_TRUE(lt<numpy>(one, 2. * one));
    EXPECT_TRUE(lt<relativeWeak>(one, 2. * one));
    EXPECT_TRUE(lt<relativeStrong>(one, 2. * one));
    EXPECT_TRUE(lt<absolute>(one, 2. * one));
    EXPECT_TRUE(lt<defaultStyle>(one, 2. * one));
  }

  void check_ge()
  {
    EXPECT_TRUE(ge(zero, zero));
    EXPECT_TRUE(ge<numpy>(zero, zero));
    EXPECT_TRUE(ge<relativeWeak>(zero, zero));
    EXPECT_TRUE(ge<relativeStrong>(zero, zero));
    EXPECT_TRUE(ge<absolute>(zero, zero));
    EXPECT_TRUE(ge<defaultStyle>(zero, zero));

    EXPECT_TRUE(ge(0.9 * epsilon, zero));
    EXPECT_TRUE(ge<numpy>(0.9 * epsilon, zero));
    EXPECT_TRUE(ge<relativeWeak>(0.9 * epsilon, zero));
    EXPECT_TRUE(ge<relativeStrong>(0.9 * epsilon, zero));
    EXPECT_TRUE(ge<absolute>(0.9 * epsilon, zero));
    EXPECT_TRUE(ge<defaultStyle>(0.9 * epsilon, zero));

    EXPECT_TRUE(ge(1.1 * epsilon, zero));
    EXPECT_TRUE(ge<numpy>(1.1 * epsilon, zero));
    EXPECT_TRUE(ge<relativeWeak>(1.1 * epsilon, zero));
    EXPECT_TRUE(ge<relativeStrong>(1.1 * epsilon, zero));
    EXPECT_TRUE(ge<absolute>(1.1 * epsilon, zero));
    EXPECT_TRUE(ge<defaultStyle>(1.1 * epsilon, zero));

    EXPECT_TRUE(ge(one, zero));
    EXPECT_TRUE(ge<numpy>(one, zero));
    EXPECT_TRUE(ge<relativeWeak>(one, zero));
    EXPECT_TRUE(ge<relativeStrong>(one, zero));
    EXPECT_TRUE(ge<absolute>(one, zero));
    EXPECT_TRUE(ge<defaultStyle>(one, zero));

    EXPECT_TRUE(ge(one + 0.9 * epsilon, one));
    EXPECT_TRUE(ge<numpy>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(ge<relativeWeak>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(ge<relativeStrong>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(ge<absolute>(one + 0.9 * epsilon, one));
    EXPECT_TRUE(ge<defaultStyle>(one + 0.9 * epsilon, one));

    EXPECT_TRUE(ge(one + 1.1 * epsilon, one));
    EXPECT_TRUE(ge<numpy>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(ge<relativeWeak>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(ge<relativeStrong>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(ge<absolute>(one + 1.1 * epsilon, one));
    EXPECT_TRUE(ge<defaultStyle>(one + 1.1 * epsilon, one));

    EXPECT_TRUE(ge(2. * one, one));
    EXPECT_TRUE(ge<numpy>(2. * one, one));
    EXPECT_TRUE(ge<relativeWeak>(2. * one, one));
    EXPECT_TRUE(ge<relativeStrong>(2. * one, one));
    EXPECT_TRUE(ge<absolute>(2. * one, one));
    EXPECT_TRUE(ge<defaultStyle>(2. * one, one));
  }

  void check_le()
  {
    EXPECT_TRUE(le(zero, zero));
    EXPECT_TRUE(le<numpy>(zero, zero));
    EXPECT_TRUE(le<relativeWeak>(zero, zero));
    EXPECT_TRUE(le<relativeStrong>(zero, zero));
    EXPECT_TRUE(le<absolute>(zero, zero));
    EXPECT_TRUE(le<defaultStyle>(zero, zero));

    EXPECT_TRUE(le(zero, 0.9 * epsilon));
    EXPECT_TRUE(le<numpy>(zero, 0.9 * epsilon));
    EXPECT_TRUE(le<relativeWeak>(zero, 0.9 * epsilon));
    EXPECT_TRUE(le<relativeStrong>(zero, 0.9 * epsilon));
    EXPECT_TRUE(le<absolute>(zero, 0.9 * epsilon));
    EXPECT_TRUE(le<defaultStyle>(zero, 0.9 * epsilon));

    EXPECT_TRUE(le(zero, 1.1 * epsilon));
    EXPECT_TRUE(le<numpy>(zero, 1.1 * epsilon));
    EXPECT_TRUE(le<relativeWeak>(zero, 1.1 * epsilon));
    EXPECT_TRUE(le<relativeStrong>(zero, 1.1 * epsilon));
    EXPECT_TRUE(le<absolute>(zero, 1.1 * epsilon));
    EXPECT_TRUE(le<defaultStyle>(zero, 1.1 * epsilon));

    EXPECT_TRUE(le(zero, one));
    EXPECT_TRUE(le<numpy>(zero, one));
    EXPECT_TRUE(le<relativeWeak>(zero, one));
    EXPECT_TRUE(le<relativeStrong>(zero, one));
    EXPECT_TRUE(le<absolute>(zero, one));
    EXPECT_TRUE(le<defaultStyle>(zero, one));

    EXPECT_TRUE(le(one, one + 0.9 * epsilon));
    EXPECT_TRUE(le<numpy>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(le<relativeWeak>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(le<relativeStrong>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(le<absolute>(one, one + 0.9 * epsilon));
    EXPECT_TRUE(le<defaultStyle>(one, one + 0.9 * epsilon));

    EXPECT_TRUE(le(one, one + 1.1 * epsilon));
    EXPECT_TRUE(le<numpy>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(le<relativeWeak>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(le<relativeStrong>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(le<absolute>(one, one + 1.1 * epsilon));
    EXPECT_TRUE(le<defaultStyle>(one, one + 1.1 * epsilon));

    EXPECT_TRUE(le(one, 2. * one));
    EXPECT_TRUE(le<numpy>(one, 2. * one));
    EXPECT_TRUE(le<relativeWeak>(one, 2. * one));
    EXPECT_TRUE(le<relativeStrong>(one, 2. * one));
    EXPECT_TRUE(le<absolute>(one, 2. * one));
    EXPECT_TRUE(le<defaultStyle>(one, 2. * one));
  }
}; // struct FloatCmpBase

template <class V, bool = VectorAbstraction<V>::has_static_size>
struct TestSize
{
  static constexpr size_t size = VectorAbstraction<V>::static_size;
};
template <class V>
struct TestSize<V, false>
{
  static constexpr size_t size = vec_size;
};

template <class S>
struct FloatCmpScalar : public FloatCmpBase<S, 0>
{
};

template <class V>
struct FloatCmpVector : public FloatCmpBase<V, TestSize<V>::size>
{
};

typedef testing::Types<double //, float
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

typedef testing::Types<std::vector<double>, Dune::FieldVector<double, vec_size>, FieldVector<double, vec_size>,
                       Dune::DynamicVector<double>, Dune::XT::LA::CommonDenseVector<double>
#if HAVE_DUNE_ISTL
                       ,
                       Dune::XT::LA::IstlDenseVector<double>
#endif
#if HAVE_EIGEN
                       ,
                       Dune::XT::LA::EigenDenseVector<double>, Dune::XT::LA::EigenMappedDenseVector<double>
#endif
                       ,
                       std::complex<double>, std::vector<std::complex<double>>,
                       Dune::FieldVector<std::complex<double>, vec_size>, FieldVector<std::complex<double>, vec_size>,
                       Dune::DynamicVector<std::complex<double>>, Dune::XT::LA::CommonDenseVector<std::complex<double>>
#if HAVE_DUNE_ISTL
                       ,
                       Dune::XT::LA::IstlDenseVector<std::complex<double>>
#endif
#if HAVE_EIGEN
                       ,
                       Dune::XT::LA::EigenDenseVector<std::complex<double>>
//                    complex just isn't possible with mapped vector
//                      , Dune::XT::LA::EigenMappedDenseVector< std::complex<double> >
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
