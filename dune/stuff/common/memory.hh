// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_MEMORY_HH
#define DUNE_STUFF_MEMORY_HH

#include <memory>
#include <boost/noncopyable.hpp>

namespace Dune {
namespace Stuff {
namespace Common {


//! make_unique implementation via herb sutter: http://herbsutter.com/gotw/_102/
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

//! just like boost::noncopyable, but for move assign/ctor
struct nonmoveable
{
  constexpr nonmoveable() = default;
  ~nonmoveable() = default;
  nonmoveable& operator=(nonmoveable&& source) = delete;
  nonmoveable(nonmoveable&& source) = delete;
};


namespace internal {


template <class T>
class ConstAccessInterface
{
public:
  virtual ~ConstAccessInterface()
  {
  }

  ConstAccessInterface<T>& operator=(const ConstAccessInterface<T>& other) = delete;
  ConstAccessInterface<T>& operator=(ConstAccessInterface<T>&& source) = delete;

  virtual const T& access() const = 0;
}; // class ConstAccessInterface


template <class T>
class ConstAccessByReference : public ConstAccessInterface<T>
{
public:
  explicit ConstAccessByReference(const T& tt)
    : tt_(tt)
  {
  }

  explicit ConstAccessByReference(const ConstAccessByReference<T>& other) = default;
  explicit ConstAccessByReference(ConstAccessByReference<T>&& source) = default;

  virtual ~ConstAccessByReference()
  {
  }

  virtual const T& access() const override final
  {
    return tt_;
  }

private:
  const T& tt_;
}; // class ConstAccessByReference


template <class T>
class ConstAccessByPointer : public ConstAccessInterface<T>
{
public:
  explicit ConstAccessByPointer(const T* tt)
    : tt_(tt)
  {
  }

  explicit ConstAccessByPointer(std::unique_ptr<const T>&& tt)
    : tt_(tt)
  {
  }

  explicit ConstAccessByPointer(std::shared_ptr<const T> tt)
    : tt_(tt)
  {
  }

  explicit ConstAccessByPointer(const ConstAccessByPointer<T>& other) = default;
  explicit ConstAccessByPointer(ConstAccessByPointer<T>&& other) = default;

  virtual ~ConstAccessByPointer()
  {
  }

  virtual const T& access() const override final
  {
    return *tt_;
  }

private:
  std::shared_ptr<const T> tt_;
}; // class ConstAccessByPointer


template <class T>
class AccessInterface
{
public:
  virtual ~AccessInterface()
  {
  }

  AccessInterface<T>& operator=(const AccessInterface<T>& other) = delete;
  AccessInterface<T>& operator=(AccessInterface<T>&& source) = delete;

  virtual T& access() = 0;
  virtual const T& access() const = 0;
}; // class AccessInterface


template <class T>
class AccessByReference : public AccessInterface<T>
{
public:
  explicit AccessByReference(T& tt)
    : tt_(tt)
  {
  }

  explicit AccessByReference(AccessByReference<T>& other) = default;
  explicit AccessByReference(AccessByReference<T>&& source) = default;

  virtual ~AccessByReference()
  {
  }

  virtual T& access() override final
  {
    return tt_;
  }

  virtual const T& access() const override final
  {
    return tt_;
  }

private:
  T& tt_;
}; // class AccessByReference


template <class T>
class AccessByPointer : public AccessInterface<T>
{
public:
  explicit AccessByPointer(T* tt)
    : tt_(tt)
  {
  }

  explicit AccessByPointer(std::unique_ptr<T>&& tt)
    : tt_(tt)
  {
  }

  explicit AccessByPointer(std::shared_ptr<T> tt)
    : tt_(tt)
  {
  }

  explicit AccessByPointer(AccessByPointer<T>& other) = default;
  explicit AccessByPointer(AccessByPointer<T>&& source) = default;

  virtual ~AccessByPointer()
  {
  }

  virtual T& access() override final
  {
    return *tt_;
  }

  virtual const T& access() const override final
  {
    return *tt_;
  }

private:
  std::shared_ptr<T> tt_;
}; // class AccessByPointer


} // namespace internal


template <class T>
class ConstStorageProvider
{
public:
  explicit ConstStorageProvider(const T& tt)
    : storage_(std::make_shared<internal::ConstAccessByReference<T>>(tt))
  {
  }

  explicit ConstStorageProvider(const T* tt)
    : storage_(std::make_shared<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::unique_ptr<const T>&& tt)
    : storage_(std::make_shared<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::shared_ptr<const T> tt)
    : storage_(std::make_shared<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(const ConstStorageProvider<T>& other) = default;
  explicit ConstStorageProvider(ConstStorageProvider<T>&& source) = default;

  ConstStorageProvider<T>& operator=(const ConstStorageProvider<T>& other) = delete;
  ConstStorageProvider<T>& operator=(ConstStorageProvider<T>&& source) = delete;

  const T& storage_access() const
  {
    return access();
  }

  const T& access() const
  {
    return storage_->access();
  }

private:
  std::shared_ptr<internal::ConstAccessInterface<T>> storage_;
}; // class ConstStorageProvider


template <class T>
class StorageProvider
{
public:
  explicit StorageProvider(T& tt)
    : storage_(std::make_shared<internal::AccessByReference<T>>(tt))
  {
  }

  explicit StorageProvider(T* tt)
    : storage_(std::make_shared<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::unique_ptr<T>&& tt)
    : storage_(std::make_shared<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::shared_ptr<T> tt)
    : storage_(std::make_shared<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(const StorageProvider<T>& other) = default;
  explicit StorageProvider(StorageProvider<T>&& source) = default;

  StorageProvider<T>& operator=(const StorageProvider<T>& other) = delete;
  StorageProvider<T>& operator=(StorageProvider<T>&& source) = delete;

  T& storage_access()
  {
    return access();
  }

  const T& storage_access() const
  {
    return access();
  }

  T& access()
  {
    return storage_->access();
  }

  const T& access() const
  {
    return storage_->access();
  }

private:
  std::shared_ptr<internal::AccessInterface<T>> storage_;
}; // class StorageProvider


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_MEMORY_HH
