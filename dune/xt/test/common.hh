// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2015)
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

typedef Dune::tuple<double, float
                    //                   , Dune::bigunsignedint
                    ,
                    int, unsigned int, unsigned long, long long, char> BasicTypes;

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
                                 const std::map<std::string, std::vector<double>>& errors_map);

void print_collected_eoc_study_results(const std::map<std::string, std::vector<double>>& results,
                                       std::ostream& out = std::cout);

// returns unsigned int on purpose, see GridProvider
unsigned int grid_elements();

} // namespace Test
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_TEST_MAIN_COMMON_HH
