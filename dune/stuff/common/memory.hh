// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_MEMORY_HH
#define DUNE_STUFF_MEMORY_HH

#include <memory>

namespace Dune {
namespace Stuff {
namespace Common {


//! make_unique implementation via herb sutter: http://herbsutter.com/gotw/_102/
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


namespace internal {


template <class T>
class AccessInterface
{
public:
  virtual ~AccessInterface()
  {
  }

  virtual T& access() = 0;
}; // class AccessInterface


template <class T>
class AccessByReference : public AccessInterface<T>
{
public:
  AccessByReference(T& tt)
    : tt_(tt)
  {
  }

  virtual ~AccessByReference()
  {
  }

  virtual T& access()
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
  AccessByPointer(T* tt)
    : tt_(tt)
  {
  }

  AccessByPointer(std::unique_ptr<T>&& tt)
    : tt_(tt)
  {
  }

  AccessByPointer(std::shared_ptr<T> tt)
    : tt_(tt)
  {
  }

  virtual ~AccessByPointer()
  {
  }

  virtual T& access()
  {
    return *tt_;
  }

private:
  std::shared_ptr<T> tt_;
}; // class AccessByPointer


} // namespace internal


template <class T>
class StorageProvider
{
public:
  StorageProvider(T& tt)
    : provide_(make_unique<internal::AccessByReference<T>>(tt))
  {
  }

  StorageProvider(T* tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  StorageProvider(std::unique_ptr<T>&& tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  StorageProvider(std::shared_ptr<T> tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  /**
   * Since the intended behaviour is unknown, call StorageProvider(new T(tt)) or StorageProvider(tt) manually!
   */
  StorageProvider(const StorageProvider<T>& other) = delete;

  StorageProvider(StorageProvider<T>&& other)
    : provide_(other.provide_)
  {
  }

  StorageProvider<T> operator=(const StorageProvider<T>& /*other*/) = delete;

  T& storage_access()
  {
    return provide_->access();
  }

  const T& storage_access() const
  {
    return provide_->access();
  }

private:
  std::unique_ptr<internal::AccessInterface<T>> provide_;
}; // class StorageProvider


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_MEMORY_HH
