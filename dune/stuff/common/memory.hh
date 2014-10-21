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

  virtual const T& access() const = 0;
}; // class ConstAccessInterface


//! \note could do noncopy/move in base, but this is more explicit imo
template <class T>
class ConstAccessByReference : public ConstAccessInterface<T>, public boost::noncopyable, public nonmoveable
{
public:
  explicit ConstAccessByReference(const T& tt)
    : tt_(tt)
  {
  }

  virtual ~ConstAccessByReference()
  {
  }

  virtual const T& access() const
  {
    return tt_;
  }

private:
  const T& tt_;
}; // class ConstAccessByReference


template <class T>
class ConstAccessByPointer : public ConstAccessInterface<T>, public boost::noncopyable, public nonmoveable
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

  virtual ~ConstAccessByPointer()
  {
  }

  virtual const T& access() const
  {
    return *tt_;
  }

private:
  const std::shared_ptr<const T> tt_;
}; // class ConstAccessByPointer


template <class T>
class AccessInterface
{
public:
  virtual ~AccessInterface()
  {
  }

  virtual T& access() = 0;

  virtual const T& access() const = 0;
}; // class AccessInterface


template <class T>
class AccessByReference : public AccessInterface<T>, public boost::noncopyable, public nonmoveable
{
public:
  explicit AccessByReference(T& tt)
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

  virtual const T& access() const
  {
    return tt_;
  }

private:
  T& tt_;
}; // class AccessByReference


template <class T>
class AccessByPointer : public AccessInterface<T>, public boost::noncopyable, public nonmoveable
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

  virtual ~AccessByPointer()
  {
  }

  virtual T& access()
  {
    return *tt_;
  }

  virtual const T& access() const
  {
    return *tt_;
  }

private:
  std::shared_ptr<T> tt_;
}; // class AccessByPointer


} // namespace internal


template <class T>
class ConstStorageProvider : public boost::noncopyable
{
public:
  explicit ConstStorageProvider(const T& tt)
    : provide_(make_unique<internal::ConstAccessByReference<T>>(tt))
  {
  }

  explicit ConstStorageProvider(const T* tt)
    : provide_(make_unique<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::unique_ptr<const T>&& tt)
    : provide_(make_unique<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::shared_ptr<const T> tt)
    : provide_(make_unique<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(ConstStorageProvider<T>&& source) = default;

  const T& storage_access() const
  {
    return access();
  }

  const T& access() const
  {
    return provide_->access();
  }

private:
  std::unique_ptr<internal::ConstAccessInterface<T>> provide_;
}; // class ConstStorageProvider


template <class T>
class StorageProvider : public boost::noncopyable, public nonmoveable
{
public:
  explicit StorageProvider(T& tt)
    : provide_(make_unique<internal::AccessByReference<T>>(tt))
  {
  }

  explicit StorageProvider(T* tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::unique_ptr<T>&& tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::shared_ptr<T> tt)
    : provide_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

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
    return provide_->access();
  }

  const T& access() const
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
