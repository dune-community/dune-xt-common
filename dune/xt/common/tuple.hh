// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012, 2014 - 2015)
//   Rene Milk       (2010 - 2013, 2015)
//   Stefan Girke    (2012)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_TUPLE_HH
#define DUNE_XT_COMMON_TUPLE_HH

#include <dune/common/tuples.hh>
#include <dune/common/typetraits.hh>

#include <boost/mpl/if.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits.hpp>

#define TMAX(t_, no_) (Dune::tuple_size<t_>::value >= (no_ + 1) ? no_ : 0)
#define TELE(t_, s_, no_) typename Dune::tuple_element<TMAX(t_, no_), t_>::type::s_

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, size_t Size>
struct RightTrimTuple
{
  typedef Dune::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 9>
{
  typedef Dune::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 8>
{
  typedef Dune::tuple<T1, T2, T3, T4, T5, T6, T7, T8> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 7>
{
  typedef Dune::tuple<T1, T2, T3, T4, T5, T6, T7> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 6>
{
  typedef Dune::tuple<T1, T2, T3, T4, T5, T6> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 5>
{
  typedef Dune::tuple<T1, T2, T3, T4, T5> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 4>
{
  typedef Dune::tuple<T1, T2, T3, T4> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 3>
{
  typedef Dune::tuple<T1, T2, T3> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 2>
{
  typedef Dune::tuple<T1, T2> type;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct RightTrimTuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, 1>
{
  typedef Dune::tuple<T1> type;
};

#define TUPLE_TYPEDEFS_2_TUPLE(t_, s_)                                                                                 \
  typename RightTrimTuple<TELE(t_, s_, 0),                                                                             \
                          TELE(t_, s_, 1),                                                                             \
                          TELE(t_, s_, 2),                                                                             \
                          TELE(t_, s_, 3),                                                                             \
                          TELE(t_, s_, 4),                                                                             \
                          TELE(t_, s_, 5),                                                                             \
                          TELE(t_, s_, 6),                                                                             \
                          TELE(t_, s_, 7),                                                                             \
                          TELE(t_, s_, 8),                                                                             \
                          Dune::tuple_size<t_>::value>::type

/**
 * @def TUPLE_TYPEDEFS_2_TUPLE( t_, s_ )
 *
 * @brief extracts types of the elements of a Dune::tuple and
 * creates a new Dune::tuple with these extracted types.
 *
 * first argument: the tuple type
 * second argument: the typedef of the elements of the tuple
 *
 * example:
 * Let two classes be given:
 *
 * @code
 * class A
 * {
 *   public:
 *   typedef int MyFancyType;
 * };
 *
 * class B
 * {
 *   public:
 *   typedef int MyFancyType;
 * };
 * @endcode
 *
 * Define a tuple, i.e.
 *
 * @code
 * typedef Dune::tuple< A, B, B >
 *   MyTuple;
 * @endcode
 *
 * Instead of writing
 *
 * @code
 * typedef Dune::tuple< typename tuple_element<0,MyTuple>::type::MyFancyType,
 *                      typename tuple_element<1,MyTuple>::type::MyFancyType,
 *                      typename tuple_element<2,MyTuple>::type::MyFancyType >
 *   MySubTupleType;
 * @endcode
 *
 * just write
 *
 * @code
 * typedef TUPLE_TYPEDEFS_2_TUPLE( MyTuple, MyFancyType )
 *   MySubTupleType;
 * @endcode
 *
 * without (general) restriction to the size
 * of the tuple @c MyTuple. Enjoy it!
 */

namespace Dune {
namespace Stuff {
namespace Common {

//! split our function wrapper into sep pointers for Datawriter input
template <class T1, class T2 = T1, class T3 = T2, class T4 = T3>
struct TupleSerializer
{
  typedef Dune::
      tuple<const typename T1::DiscreteVelocityFunctionType*, const typename T1::DiscretePressureFunctionType*,
            const typename T2::DiscreteVelocityFunctionType*, const typename T2::DiscretePressureFunctionType*,
            const typename T3::DiscreteVelocityFunctionType*, const typename T3::DiscretePressureFunctionType*,
            const typename T4::DiscreteVelocityFunctionType*, const typename T4::DiscretePressureFunctionType*>
          TupleType;

  static TupleType& getTuple(T1& t1, T2& t2, T3& t3, T4& t4)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(&(t1.discreteVelocity()),
                                 &(t1.discretePressure()),
                                 &(t2.discreteVelocity()),
                                 &(t2.discretePressure()),
                                 &(t3.discreteVelocity()),
                                 &(t3.discretePressure()),
                                 &(t4.discreteVelocity()),
                                 &(t4.discretePressure()));

    return *t;
  } // getTuple

  static TupleType& getTuple(T1& t1, T2& t2, T3& t3)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(&(t1.discreteVelocity()),
                                 &(t1.discretePressure()),
                                 &(t2.discreteVelocity()),
                                 &(t2.discretePressure()),
                                 &(t3.discreteVelocity()),
                                 &(t3.discretePressure()),
                                 nullptr,
                                 nullptr);

    return *t;
  } // getTuple

  static TupleType& getTuple(T1& t1, T2& t2)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(&(t1.discreteVelocity()),
                                 &(t1.discretePressure()),
                                 &(t2.discreteVelocity()),
                                 &(t2.discretePressure()),
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 nullptr);

    return *t;
  } // getTuple

  static TupleType& getTuple(T1& t1)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(
        &(t1.discreteVelocity()), &(t1.discretePressure()), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

    return *t;
  } // getTuple
};

template <class T1, class T2 = T1, class T3 = T1, class T4 = T1, class T5 = T1, class T6 = T1, class T7 = T1,
          class T8 = T1, class T9 = T1>
struct FullTuple : public Dune::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>
{
  FullTuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
    : Dune::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>(t1, t2, t3, t4, t5, t6, t7, t8, t9)
  {
  }
};

// reduced from
// http://stackoverflow.com/questions/1492204/is-it-possible-to-generate-types-with-all-combinations-of-template-arguments
namespace TupleProduct {
using boost::is_same;
using boost::mpl::begin;
using boost::mpl::end;
using boost::mpl::next;
using boost::mpl::if_;
using boost::mpl::deref;

size_t total_recursions = 0;

struct end_of_recursion_tag
{
  template <class... Args>
  static void Run(Args&&... /*args*/)
  {
    std::cout << "end of " << total_recursions << " recursions\n";
  }
};

template <class UTypes, // Forward Sequence, e.g. boost::mpl::vector
          class VTypes, // Forward Sequence, e.g. boost::mpl::vector
          class TestFunc // class type that has a nested templated run() member function
          >
struct Combine
{
  // forward declaration
  template <class UIterator, class VIterator>
  struct Generate;

  // this class implements recursion body
  template <class UIterator, class VIterator>
  struct Next
  {
    // u_begin is not necessary ;)
    // it would be cheaper not to pre-declare all of them since we force evaluation
    // however this dramatically increase the readability
    typedef typename begin<VTypes>::type v_begin;

    typedef typename end<UTypes>::type u_end;
    typedef typename end<VTypes>::type v_end;

    typedef typename next<UIterator>::type u_next;
    typedef typename next<VIterator>::type v_next;

    typedef
        typename if_<boost::is_same<v_next, v_end>,
                     typename if_<boost::is_same<u_next, u_end>, end_of_recursion_tag, Generate<u_next, v_begin>>::type,
                     Generate<UIterator, v_next>>::type type;
  };

  //  this class run test on generated types in thos round and go to next*/
  template <class UIterator = typename begin<UTypes>::type, class VIterator = typename begin<VTypes>::type>
  struct Generate
  {
    //  generate <<next>> target type
    typedef typename Next<UIterator, VIterator>::type next_type;

    static void Run()
    {
      // increment recursion counter
      ++total_recursions;

      // test on the generated types of this round of recursion
      TestFunc::template run<typename deref<UIterator>::type, typename deref<VIterator>::type>();

      // go to the next round of recursion
      next_type::Run();
    }

    template <class... Args>
    static void Run(Args&&... args)
    {
      // increment recursion counter
      ++total_recursions;

      // test on the generated types of this round of recursion
      TestFunc::template run<typename deref<UIterator>::type, typename deref<VIterator>::type>(
          std::forward<Args>(args)...);

      // go to the next round of recursion
      next_type::Run(std::forward<Args>(args)...);
    }
  };
};

} // namespace TupleProduct

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_XT_COMMON_TUPLE_HH
