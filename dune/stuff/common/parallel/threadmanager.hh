// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_THREADMANAGER_HH
#define DUNE_STUFF_COMMON_THREADMANAGER_HH

#include <vector>
#include <algorithm>
#include <type_traits>
#if HAVE_TBB
#include <tbb/enumerable_thread_specific.h>
#endif
#include <boost/noncopyable.hpp>

#include <dune/stuff/common/type_utils.hh>

namespace Dune {
namespace Stuff {

/** abstractions of threading functionality
 *  currently forwads to dune-fem if possible, falls back to single-thread dummy imp
 **/
struct ThreadManager
{
  //! return maximal number of threads possbile in the current run
  static unsigned int max_threads();

  //! return number of current threads
  static unsigned int current_threads();

  //! return thread number
  static unsigned int thread();

  //! set maximal number of threads available during run
  static void set_max_threads(const unsigned int count);

private:
  //  std::unique_ptr<tbb::task_scheduler_init> tbb_init_;
};

/** Automatic Storage of non-static, N thread-local values
 **/
template <class ValueImp>
class FallbackPerThreadValue : public boost::noncopyable
{
public:
  typedef ValueImp ValueType;
  static_assert(std::is_copy_assignable<ValueImp>::value || std::is_move_constructible<ValueImp>::value,
                "ValueImp not usable in a std::vector");

private:
  typedef FallbackPerThreadValue<ValueImp> ThisType;
  typedef std::vector<ValueType> ContainerType;

public:
  //! Initialization by copy construction of ValueType
  explicit FallbackPerThreadValue(const ValueType& value)
    : values_(ThreadManager::max_threads(), value)
  {
  }

  FallbackPerThreadValue(const FallbackPerThreadValue<ValueType>& other) = default;
  FallbackPerThreadValue(FallbackPerThreadValue<ValueType>&& other) = default;
  FallbackPerThreadValue& operator=(const FallbackPerThreadValue<ValueType>& other) = default;
  FallbackPerThreadValue& operator=(FallbackPerThreadValue<ValueType>&& other) = default;

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit FallbackPerThreadValue(InitTypes&&... ctor_args)
    : values_(ThreadManager::max_threads(), ValueType(std::forward<InitTypes>(ctor_args)...))
  {
  }

  ThisType& operator=(const ValueType& value)
  {
    values_ = ContainerType(ThreadManager::max_threads(), value);
    return *this;
  }


  operator ValueImp() const
  {
    return this->operator*();
  }

  ValueType& operator*()
  {
    return values_[ThreadManager::thread()];
  }

  const ValueType& operator*() const
  {
    return values_[ThreadManager::thread()];
  }

  ValueType* operator->()
  {
    return &values_[ThreadManager::thread()];
  }

  const ValueType* operator->() const
  {
    return &values_[ThreadManager::thread()];
  }

  template <class BinaryOperation>
  ValueType accumulate(ValueType init, BinaryOperation op) const
  {
    return std::accumulate(values_.begin(), values_.end(), init, op);
  }

  ValueType sum() const
  {
    return accumulate(ValueType(0), std::plus<ValueType>());
  }

private:
  ContainerType values_;
};

#if HAVE_TBB
/** Automatic Storage of non-static, N thread-local values
 **/
template <class ValueImp>
class TBBPerThreadValue : public boost::noncopyable
{
public:
  typedef ValueImp ValueType;
  static_assert(std::is_copy_assignable<ValueImp>::value || std::is_move_constructible<ValueImp>::value,
                "ValueImp not usable in a std::vector");

private:
  typedef TBBPerThreadValue<ValueImp> ThisType;
  typedef tbb::enumerable_thread_specific<ValueType> ContainerType;

public:
  //! Initialization by copy construction of ValueType
  explicit TBBPerThreadValue(const ValueType& value)
    : values_(value)
  {
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit TBBPerThreadValue(InitTypes&&... ctor_args)
    : values_(ValueType(std::forward<InitTypes>(ctor_args)...))
  {
  }

  ThisType& operator=(const ValueType& value)
  {
    values_ = ContainerType(value);
    return *this;
  }

  operator ValueImp() const
  {
    return this->operator*();
  }

  ValueType& operator*()
  {
    return values_.local();
  }

  const ValueType& operator*() const
  {
    return values_.local();
  }

  ValueType* operator->()
  {
    return &values_.local();
  }

  const ValueType* operator->() const
  {
    return &values_.local();
  }

  template <class BinaryOperation>
  ValueType accumulate(ValueType /*init*/, BinaryOperation op) const
  {
    return values_.combine(op);
  }

  ValueType sum() const
  {
    return values_.combine(std::plus<ValueType>());
  }

private:
  mutable ContainerType values_;
};

template <typename T>
using PerThreadValue = TBBPerThreadValue<T>;
#else // HAVE_TBB
template <typename T>
using PerThreadValue = FallbackPerThreadValue<T>;
#endif
}
}

#endif // DUNE_STUFF_COMMON_THREADMANAGER_HH
