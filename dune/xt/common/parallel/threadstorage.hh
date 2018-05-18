// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016 - 2018)
//   Rene Milk       (2013 - 2016, 2018)

#ifndef DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH
#define DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH

#if HAVE_TBB
#include <tbb/enumerable_thread_specific.h>
#endif

#include <algorithm>
#include <list>
#include <numeric>
#include <type_traits>

namespace Dune {
namespace XT {
namespace Common {
namespace internal {


#if HAVE_TBB

template <class ValueImp>
class EnumerableThreadSpecificWrapper
{
  // enumerable_thread_specific does not compile with ConstValueType as template param
  using BackendType = typename tbb::enumerable_thread_specific<std::remove_const_t<ValueImp>>;

public:
  using ValueType = ValueImp;
  using ConstValueType = std::add_const_t<ValueType>;
  using iterator = typename BackendType::iterator;
  using const_iterator = typename BackendType::const_iterator;

  template <class... InitTypes>
  explicit EnumerableThreadSpecificWrapper(InitTypes&&... ctor_args)
    : values_(std::forward<InitTypes>(ctor_args)...)
  {
  }

  ValueType& local()
  {
    return values_.local();
  }

  // tbb does not provide a const version of local (as elements may be inserted when a new thread accesses values_), so
  // values_ has to be mutable
  const ValueType& local() const
  {
    return values_.local();
  }

  typename BackendType::iterator begin()
  {
    return values_.begin();
  }

  typename BackendType::iterator end()
  {
    return values_.end();
  }

  typename BackendType::const_iterator begin() const
  {
    return values_.begin();
  }

  typename BackendType::const_iterator end() const
  {
    return values_.end();
  }

  template <class BinaryOperation>
  ValueType combine(BinaryOperation op) const
  {
    return values_.combine(op);
  }

private:
  mutable BackendType values_;
}; // class EnumerableThreadSpecificWrapper<ValueImp>

#else // HAVE_TBB

template <class ValueImp>
class EnumerableThreadSpecificWrapper
{
  using BackendType = std::array<std::remove_const_t<ValueImp>, 1>;

public:
  using ValueType = ValueImp;
  using ConstValueType = std::add_const_t<ValueType>;
  using iterator = typename BackendType::iterator;
  using const_iterator = typename BackendType::const_iterator;

  //! Initialization by copy construction of ValueType
  explicit EnumerableThreadSpecificWrapper(ConstValueType& value)
    : values_{value}
  {
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit EnumerableThreadSpecificWrapper(InitTypes&&... ctor_args)
    : values_{ValueType(std::forward<InitTypes>(ctor_args)...)}
  {
  }

  ValueType& local()
  {
    return values_[0];
  }

  const ValueType& local() const
  {
    return values_[0];
  }

  iterator begin()
  {
    return values_.begin();
  }

  iterator end()
  {
    return values_.end();
  }

  const_iterator begin() const
  {
    return values_.begin();
  }

  const_iterator end() const
  {
    return values_.end();
  }

  template <class BinaryOperation>
  ValueType combine(BinaryOperation /*op*/) const
  {
    return values_[0];
  }

private:
  BackendType values_;
}; // class EnumerableThreadSpecificWrapper<ValueImp>

#endif // HAVE_TBB


} // namespace interal


/** Automatic Storage of non-static, N thread-local values
 **/
template <class ValueImp>
class PerThreadValue
{
  using ContainerType = internal::EnumerableThreadSpecificWrapper<ValueImp>;

public:
  using ValueType = typename ContainerType::ValueType;
  using ConstValueType = typename ContainerType::ConstValueType;

  //! Initialization by copy construction of ValueType
  explicit PerThreadValue(ConstValueType& value)
    : values_(value)
  {
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit PerThreadValue(InitTypes&&... ctor_args)
    : values_(std::forward<InitTypes>(ctor_args)...)
  {
  }

  operator ValueType() const
  {
    return values_.local();
  }

  ValueType& operator*()
  {
    return values_.local();
  }

  ConstValueType& operator*() const
  {
    return values_.local();
  }

  ValueType* operator->()
  {
    return &values_.local();
  }

  ConstValueType* operator->() const
  {
    return &values_.local();
  }

  template <class BinaryOperation>
  ValueType accumulate(ValueType init, BinaryOperation op) const
  {
    return op(init, values_.combine(op));
  }

  ValueType sum() const
  {
    return accumulate(ValueType(0), std::plus<ValueType>());
  }

  typename ContainerType::iterator begin()
  {
    return values_.begin();
  }
  typename ContainerType::iterator end()
  {
    return values_.end();
  }

  typename ContainerType::const_iterator begin() const
  {
    return values_.begin();
  }
  typename ContainerType::const_iterator end() const
  {
    return values_.end();
  }

private:
  ContainerType values_;
}; // class PerThreadValue<ValueImp>


template <class Imp, typename Result, class Reduction = std::plus<Result>>
class ThreadResultPropagator
{
public:
  ThreadResultPropagator(Imp* imp)
    : imp_(imp)
    , copies_({imp})
  {
  }

  Imp* copy_imp()
  {
    auto cpy = new Imp(*imp_);
    cpy->copies_.push_back(cpy);
    assert(cpy->copies_.size() > 1);
    return cpy;
  }

  void finalize_imp()
  {
    Reduction reduce;
    Result result{0};
    Imp* cpy = nullptr;
    while (copies_.size() > 0) {
      cpy = copies_.back();
      copies_.pop_back();
      if (cpy != nullptr) {
        result = reduce(result, cpy->result());
      }
    }
    cpy->set_result(result);
  }

private:
  Imp* imp_;
  std::list<Imp*> copies_;
};


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH
