#ifndef DUNE_STUFF_TUPLE_HH
#define DUNE_STUFF_TUPLE_HH

#include <dune/common/tuples.hh>

namespace Stuff {

//! split our function wrapper into sep pointers for Datawriter input
template <class T1, class T2 = T1, class T3 = T2, class T4 = T3>
struct TupleSerializer
{
  typedef Dune::
      Tuple<const typename T1::DiscreteVelocityFunctionType*, const typename T1::DiscretePressureFunctionType*,
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
  }
  static TupleType& getTuple(T1& t1, T2& t2, T3& t3)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(&(t1.discreteVelocity()),
                                 &(t1.discretePressure()),
                                 &(t2.discreteVelocity()),
                                 &(t2.discretePressure()),
                                 &(t3.discreteVelocity()),
                                 &(t3.discretePressure()),
                                 NULL,
                                 NULL);
    return *t;
  }
  static TupleType& getTuple(T1& t1, T2& t2)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t = new TupleType(&(t1.discreteVelocity()),
                                 &(t1.discretePressure()),
                                 &(t2.discreteVelocity()),
                                 &(t2.discretePressure()),
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL);
    return *t;
  }
  static TupleType& getTuple(T1& t1)
  {
    // yay for dangling pointers, but using a local static here fubared sequential runs with diff grid
    TupleType* t =
        new TupleType(&(t1.discreteVelocity()), &(t1.discretePressure()), NULL, NULL, NULL, NULL, NULL, NULL);
    return *t;
  }
};

template <class T1, class T2 = T1, class T3 = T1, class T4 = T1, class T5 = T1, class T6 = T1, class T7 = T1,
          class T8 = T1, class T9 = T1>
struct FullTuple : public Dune::Tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>
{
  FullTuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
    : Dune::Tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>(t1, t2, t3, t4, t5, t6, t7, t8, t9)
  {
  }
};

template <class T1>
Dune::Tuple<T1> makeTuple(T1& t1)
{
  return Dune::Tuple<T1>(t1);
}

template <class T1, class T2>
Dune::Tuple<T1, T2> makeTuple(T1& t1, T2& t2)
{
  return Dune::Tuple<T1, T2>(t1, t2);
}

} // namespace Stuff {
#endif // DUNE_STUFF_TUPLE_HH
