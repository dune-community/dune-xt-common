#ifndef DUNE_STUFF_TEST_FLOAT_COMPARE_HH
#define DUNE_STUFF_TEST_FLOAT_COMPARE_HH

#include <type_traits>
#include <complex>

#include <dune/stuff/aliases.hh>
#include <dune/stuff/test/gtest/gtest.h>
#include <dune/stuff/common/float_cmp.hh>


#define DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(ID, id)                                                             \
  template <DSCFl::Style style, class FirstType, class SecondType>                                                     \
  typename std::                                                                                                       \
      enable_if<DSCFl::internal::cmp_type_check<FirstType, SecondType, typename DSCFl::MT<FirstType>::T>::value,       \
                void>::type                                                                                            \
          DSC_EXPECT_FLOAT_##ID(const FirstType& expected,                                                             \
                                const SecondType& actual,                                                              \
                                const typename DSCFl::MT<FirstType>::Eps& rtol =                                       \
                                    DSCFl::DefaultEpsilon<typename DSCFl::MT<FirstType>::T, style>::value(),           \
                                const typename DSCFl::MT<FirstType>::Eps& atol =                                       \
                                    DSCFl::DefaultEpsilon<typename DSCFl::MT<FirstType>::T, style>::value())           \
  {                                                                                                                    \
    const auto id = DSCFl::internal::Call<FirstType, SecondType, typename DSCFl::MT<FirstType>::Eps, style>::id;       \
    EXPECT_PRED4(id, expected, actual, rtol, atol);                                                                    \
  }                                                                                                                    \
                                                                                                                       \
  template <class FirstType, class SecondType>                                                                         \
  typename std::                                                                                                       \
      enable_if<DSCFl::internal::cmp_type_check<FirstType, SecondType, typename DSCFl::MT<FirstType>::T>::value,       \
                void>::type                                                                                            \
          DSC_EXPECT_FLOAT_##ID(                                                                                       \
              const FirstType& first,                                                                                  \
              const SecondType& second,                                                                                \
              const typename DSCFl::MT<FirstType>::Eps& rtol =                                                         \
                  DSCFl::DefaultEpsilon<typename DSCFl::MT<FirstType>::T, DSCFl::Style::defaultStyle>::value(),        \
              const typename DSCFl::MT<FirstType>::Eps& atol =                                                         \
                  DSCFl::DefaultEpsilon<typename DSCFl::MT<FirstType>::T, DSCFl::Style::defaultStyle>::value())        \
  {                                                                                                                    \
    DSC_EXPECT_FLOAT_##ID<DSCFl::Style::defaultStyle>(first, second, rtol, atol);                                      \
  }

DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(EQ, eq)
DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(NE, ne)
DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(GT, gt)
DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(LT, lt)
DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(GE, ge)
DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR(LE, le)
#undef DUNE_STUFF_COMMON_FLOAT_TEST_CMP_GENERATOR

#endif // DUNE_STUFF_TEST_FLOAT_COMPARE_HH
