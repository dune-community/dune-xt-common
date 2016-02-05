// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014, 2016)
//   Rene Milk       (2013 - 2015)

#ifndef DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH
#define DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH

#include <deque>
#include <algorithm>
#include <type_traits>
#if HAVE_TBB
#include <tbb/enumerable_thread_specific.h>
#endif
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
class FallbackPerThreadValue : public boost::noncopyable
{
public:
  typedef ValueImp ValueType;
  typedef typename std::conditional<std::is_const<ValueImp>::value, ValueImp, const ValueImp>::type ConstValueType;

private:
  typedef FallbackPerThreadValue<ValueImp> ThisType;
  typedef std::deque<std::unique_ptr<ValueType>> ContainerType;

public:
  //! Initialization by copy construction of ValueType
  explicit FallbackPerThreadValue(ConstValueType& value)
    : values_(threadManager().max_threads())
  {
    std::generate(values_.begin(), values_.end(), [=]() { return make_unique<ValueType>(value); });
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit FallbackPerThreadValue(InitTypes&&... ctor_args)
    : values_(threadManager().max_threads())
  {
#if __GNUC__
    // cannot unpack in lambda due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
    ValueType v(ctor_args...);
    std::generate(values_.begin(), values_.end(), [&]() { return make_unique<ValueType>(v); });
#else
    std::generate(values_.begin(), values_.end(), [&]() { return make_unique<ValueType>(ctor_args...); });
#endif
  }

  ThisType& operator=(ConstValueType&& value)
  {
    std::generate(values_.begin(), values_.end(), [=]() { return make_unique<ValueType>(value); });
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
  typedef typename std::conditional<std::is_const<ValueImp>::value, ValueImp, const ValueImp>::type ConstValueType;

private:
  typedef TBBPerThreadValue<ValueImp> ThisType;
  typedef tbb::enumerable_thread_specific<std::unique_ptr<ValueType>> ContainerType;

public:
  //! Initialization by copy construction of ValueType
  explicit TBBPerThreadValue(ValueType value)
    : values_(new ContainerType([=]() { return make_unique<ValueType>(value); }))
  {
  }

  //! Initialization by in-place construction ValueType with \param ctor_args
  template <class... InitTypes>
  explicit TBBPerThreadValue(InitTypes&&... ctor_args)
#if __GNUC__
      // cannot unpack in lambda due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
      : TBBPerThreadValue(ValueType(ctor_args...))
#else
    : values_(new ContainerType([=]() { return make_unique<ValueType>(ctor_args...); }))
#endif
  {
  }

  ThisType& operator=(ValueType&& value)
  {
    values_ = make_unique<ContainerType>([=]() { return make_unique<ValueType>(value); });
    return *this;
  }

  operator ValueImp() const
  {
    return this->operator*();
  }

  ValueType& operator*()
  {
    return *values_->local();
  }

  ConstValueType& operator*() const
  {
    return *values_->local();
  }

  ValueType* operator->()
  {
    return values_->local().get();
  }

  ConstValueType* operator->() const
  {
    return values_->local().get();
  }

  template <class BinaryOperation>
  ValueType accumulate(ValueType init, BinaryOperation op) const
  {
    typedef const typename ContainerType::value_type ptr;
    auto l = [&](ConstValueType& a, ptr& b) { return op(a, *b); };
    return std::accumulate(values_->begin(), values_->end(), init, l);
  }

  ValueType sum() const
  {
    return accumulate(ValueType(), std::plus<ValueType>());
  }

private:
  mutable std::unique_ptr<ContainerType> values_;
};

template <typename T>
using PerThreadValue = TBBPerThreadValue<T>;
#else // HAVE_TBB
template <typename T>
using PerThreadValue = FallbackPerThreadValue<T>;
#endif

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PARALLEL_THREADSTORAGE_HH
