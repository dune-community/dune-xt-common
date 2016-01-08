// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Rene Milk (2015)

#ifndef DUNE_XT_COMMON_TEST_FLOAT_COMPARE_HH
#define DUNE_XT_COMMON_TEST_FLOAT_COMPARE_HH

#include <type_traits>
#include <complex>

#include <dune/xt/test/gtest/gtest.h>
#include <dune/xt/common/float_cmp.hh>

#define DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(ID, id)                                                                \
  template <Dune::XT::Common::FloatCmp::Style style, class FirstType, class SecondType>                                \
  typename std::enable_if<Dune::XT::Common::FloatCmp::internal::                                                       \
                              cmp_type_check<FirstType,                                                                \
                                             SecondType,                                                               \
                                             typename Dune::XT::Common::FloatCmp::MT<FirstType>::T>::value,            \
                          void>::type                                                                                  \
      DXTC_EXPECT_FLOAT_##ID(                                                                                          \
          const FirstType& expected,                                                                                   \
          const SecondType& actual,                                                                                    \
          const typename Dune::XT::Common::FloatCmp::MT<FirstType>::Eps& rtol =                                        \
              Dune::XT::Common::FloatCmp::DefaultEpsilon<typename Dune::XT::Common::FloatCmp::MT<FirstType>::T,        \
                                                         style>::value(),                                              \
          const typename Dune::XT::Common::FloatCmp::MT<FirstType>::Eps& atol =                                        \
              Dune::XT::Common::FloatCmp::DefaultEpsilon<typename Dune::XT::Common::FloatCmp::MT<FirstType>::T,        \
                                                         style>::value())                                              \
  {                                                                                                                    \
    const auto id = Dune::XT::Common::FloatCmp::internal::                                                             \
        Call<FirstType, SecondType, typename Dune::XT::Common::FloatCmp::MT<FirstType>::Eps, style>::id;               \
    EXPECT_PRED4(id, expected, actual, rtol, atol);                                                                    \
  }                                                                                                                    \
                                                                                                                       \
  template <class FirstType, class SecondType>                                                                         \
  typename std::enable_if<Dune::XT::Common::FloatCmp::internal::                                                       \
                              cmp_type_check<FirstType,                                                                \
                                             SecondType,                                                               \
                                             typename Dune::XT::Common::FloatCmp::MT<FirstType>::T>::value,            \
                          void>::type                                                                                  \
      DXTC_EXPECT_FLOAT_##ID(                                                                                          \
          const FirstType& first,                                                                                      \
          const SecondType& second,                                                                                    \
          const typename Dune::XT::Common::FloatCmp::MT<FirstType>::Eps& rtol =                                        \
              Dune::XT::Common::FloatCmp::DefaultEpsilon<typename Dune::XT::Common::FloatCmp::MT<FirstType>::T,        \
                                                         Dune::XT::Common::FloatCmp::Style::defaultStyle>::value(),    \
          const typename Dune::XT::Common::FloatCmp::MT<FirstType>::Eps& atol =                                        \
              Dune::XT::Common::FloatCmp::DefaultEpsilon<typename Dune::XT::Common::FloatCmp::MT<FirstType>::T,        \
                                                         Dune::XT::Common::FloatCmp::Style::defaultStyle>::value())    \
  {                                                                                                                    \
    DXTC_EXPECT_FLOAT_##ID<Dune::XT::Common::FloatCmp::Style::defaultStyle>(first, second, rtol, atol);                \
  }

DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(EQ, eq)
DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(NE, ne)
DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(GT, gt)
DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(LT, lt)
DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(GE, ge)
DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR(LE, le)
#undef DUNE_XT_COMMON_FLOAT_TEST_CMP_GENERATOR

#endif // DUNE_XT_COMMON_TEST_FLOAT_COMPARE_HH
