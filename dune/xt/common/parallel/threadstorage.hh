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

#include <deque>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <numeric>
#include <list>
#include <functional>

#include <boost/noncopyable.hpp>

#include <dune/xt/common/type_traits.hh>
#include <dune/xt/common/memory.hh>
#include <dune/xt/common/parallel/threadmanager.hh>

namespace Dune {
namespace XT {
namespace Common {

/** Automatic Storage of non-static, N thread-local values
 **/
template <class ValueImp>
class PerThreadValue : public boost::noncopyable
{
public:
  typedef ValueImp ValueType;
  typedef typename std::conditional<std::is_const<ValueImp>::value, ValueImp, const ValueImp>::type ConstValueType;

private:
  typedef PerThreadValue<ValueImp> ThisType;
  typedef std::deque<std::unique_ptr<ValueType>> ContainerType;

public:
  //! Initialization by copy construction of ValueType
  explicit PerThreadValue(ConstValueType& value)
    : values_(threadManager().max_threads())
  {
    std::generate(values_.begin(), values_.end(), [=]() { return Common::make_unique<ValueType>(value); });
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit PerThreadValue(InitTypes&&... ctor_args)
    : values_(threadManager().max_threads())
  {
    for (auto&& val : values_)
      val = Common::make_unique<ValueType>(ctor_args...);
  }

  ThisType& operator=(ConstValueType&& value)
  {
    std::generate(values_.begin(), values_.end(), [=]() { return Common::make_unique<ValueType>(value); });
    return *this;
  }

  operator ValueType() const
  {
    return this->operator*();
  }

  ValueType& operator*()
  {
    return *values_[threadManager().thread()];
  }

  ConstValueType& operator*() const
  {
    return *values_[threadManager().thread()];
  }

  ValueType* operator->()
  {
    return values_[threadManager().thread()].get();
  }

  ConstValueType* operator->() const
  {
    return values_[threadManager().thread()].get();
  }

  template <class BinaryOperation>
  ValueType accumulate(ValueType init, BinaryOperation op) const
  {
    typedef const typename ContainerType::value_type ptr;
    auto l = [&](ConstValueType& a, ptr& b) { return op(a, *b); };
    return std::accumulate(values_.begin(), values_.end(), init, l);
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
};

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
