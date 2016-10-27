// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2015)

#ifndef DUNE_XT_COMMON_TEST_MAIN_COMMON_HH
#define DUNE_XT_COMMON_TEST_MAIN_COMMON_HH

#include <string>
#include <map>
#include <vector>
#include <utility>

#include <dune/common/tuples.hh>
#include <dune/common/tupleutility.hh>

#include <dune/xt/common/convergence-study.hh>
#include <dune/xt/common/compiler.hh>
#include <dune/xt/common/vector.hh>

template <template <class> class Test>
struct TestRunner
{
  struct Visitor
  {
    template <class T>
    void visit(const T&)
    {
      Test<T>().run();
    }
  };

  template <class Tuple>
  static void run()
  {
    Tuple t;
    Dune::ForEachValue<Tuple> fe(t);
    Visitor v;
    fe.apply(v);
  }
}; // struct TestRunner

template <int i>
struct Int
{
  static const int value = i;
};

typedef Dune::tuple<double,
                    float
                    //                   , Dune::bigunsignedint
                    ,
                    int,
                    unsigned int,
                    unsigned long,
                    long long,
                    char>
    BasicTypes;

//! where sleep only counts toward wall time, this wastes actual cpu time
void busywait(const size_t ms);

namespace Dune {
namespace XT {
namespace Test {
namespace internal {

std::pair<size_t, ssize_t> convert_to_scientific(const double number, const size_t precision = 2);

std::string print_vector(const std::vector<double>& vec);

} // namespace internal

void check_eoc_study_for_success(const Common::ConvergenceStudy& study,
                                 const std::map<std::string, std::vector<double>>& errors_map,
                                 const double& zero_tolerance = 1e-15);

void print_collected_eoc_study_results(const std::map<std::string, std::vector<double>>& results,
                                       std::ostream& out = std::cout);

// returns unsigned int on purpose, see GridProvider
unsigned int grid_elements();

template <typename T>
static typename std::enable_if<Common::is_vector<T>::value, T>::type
init_bound(typename Common::VectorAbstraction<T>::S val)
{
  const auto size = Common::VectorAbstraction<T>::has_static_size ? Common::VectorAbstraction<T>::static_size : 3u;
  return Common::VectorAbstraction<T>::create(size, val);
}
template <typename T>
static typename std::enable_if<!Common::is_vector<T>::value, T>::type
init_bound(typename Common::VectorAbstraction<T>::S val)
{
  return T(val);
}
} // namespace Test
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_TEST_MAIN_COMMON_HH
