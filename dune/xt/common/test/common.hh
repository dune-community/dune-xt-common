// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   Rene Milk       (2015 - 2016, 2018)
//   Tobias Leibner  (2016 - 2017)

#ifndef DUNE_XT_COMMON_TEST_MAIN_COMMON_HH
#define DUNE_XT_COMMON_TEST_MAIN_COMMON_HH

#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <ostream>

#include <dune/common/hybridutilities.hh>
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
    Visitor v;
    Dune::Hybrid::forEach(Dune::Std::make_index_sequence<std::tuple_size<Tuple>::value>{},
                          [&](auto i) { v.visit(std::get<i>(t)); });
  }
}; // struct TestRunner

template <int i>
struct Int
{
  static const int value = i;
};

typedef std::tuple<double,
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
namespace std {


template <class T>
ostream& operator<<(ostream& out, const vector<T>& results)
{
  if (results.size() == 0)
    out << "{}";
  else if (results.size() == 1)
    out << "{" << *results.begin() << "}";
  else {
    auto iterator = results.begin();
    out << "{" << *iterator;
    ++iterator;
    for (; iterator != results.end(); ++iterator) {
      out << ", " << *iterator;
    }
    out << "}";
  }
  return out;
}

template <class T>
ostream& operator<<(ostream& out, const set<T>& results)
{
  if (results.size() == 0)
    out << "{}";
  else if (results.size() == 1)
    out << "{" << *results.begin() << "}";
  else {
    auto iterator = results.begin();
    out << "{" << *iterator;
    ++iterator;
    for (; iterator != results.end(); ++iterator) {
      out << ", " << *iterator;
    }
    out << "}";
  }
  return out;
}

template <class L, class R>
ostream& operator<<(ostream& out, const pair<L, R>& results)
{
  out << "{" << results.first << ", " << results.second << "}";
  return out;
}

template <class F, class S>
ostream& operator<<(ostream& out, const map<F, S>& results)
{
  if (results.size() == 0)
    out << "{}" << endl;
  else if (results.size() == 1)
    out << "{{" << results.begin()->first << ", " << results.begin()->second << "}}";
  else {
    auto iterator = results.begin();
    out << "{{" << iterator->first << ", " << iterator->second << "}";
    ++iterator;
    for (; iterator != results.end(); ++iterator) {
      out << ",\n {" << iterator->first << ", " << iterator->second << "}";
    }
    out << "}";
  }
  return out;
}


} // namespace std

#endif // DUNE_XT_COMMON_TEST_MAIN_COMMON_HH
