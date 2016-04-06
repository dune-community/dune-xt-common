// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014 - 2016)
//   Rene Milk       (2011 - 2012, 2014 - 2015)

#ifndef DUNE_XT_COMMON_MEMORY_HH
#define DUNE_XT_COMMON_MEMORY_HH

#include <cassert>
#include <memory>
#include <boost/noncopyable.hpp>

namespace Dune {
namespace XT {
namespace Common {

//! make_unique implementation via herb sutter: http://herbsutter.com/gotw/_102/
//! \TODO this can be delegated to stdlib with c++14
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
  /**
   * \attention This ctor transfers ownership to ConstAccessByPointer, do not delete tt manually!
   */
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
  /**
   * \attention This ctor transfers ownership to AccessByPointer, do not delete tt manually!
   */
  explicit AccessByPointer(T* tt)
    : tt_(tt)
  {
  }

  explicit AccessByPointer(std::unique_ptr<T>&& tt)
    : tt_(tt)
  {
  }

  explicit AccessByPointer(std::shared_ptr<T> tt)
    : tt_(tt.get())
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


/**
 * \brief Provides generic (const) access to objects of different origins.
 *
 * Cosider the following base class which always requires a vector:
\code
struct Base
{
  Base(const std::vector<double>& vec);
};
\endcode
 * Consider further a derived class which should be constructible with a given vector as well as without:
\code
struct Derived : public Base
{
  Derived(const std::vector<double>& vec);
  Derived();
}
\endcode
 * In the latter case, a vector should be created automatically, which is problematic due to the requirements of Base.
 * The solution is to first derive from ConstStorageProvider or StorageProvider, which handles the management of the
 * vector:
\code
struct Derived
 : private ConstStorageProvider<std::vector<double>>
 , public Base
{
  typedef ConstStorageProvider<std::vector<double>> VectorProvider;

  Derived(const std::vector<double>& vec)
    : VectorProvider(vec)
    , Base(VectorProvider::access())
  {}

  Derived()
    : VectorProvider(new std::vector<double>())
    , Base(VectorProvider::access())
}
\endcode
 * For the latter to work, ConstStorageProvider (as well as StorageProvider) needs to take ownership of the provided raw
 * pointer.
 * \attention ConstStorageProvider (as well as StorageProvider) takes ownership of the provided raw pointer. Thus, the
 *            following code is supposed to fail:
\code
const T* tt = new T();
{
  ConstStorageProvider<T> provider(tt);
}
const T& derefed_tt = *tt;
\endcode
 *            Do the following instead:
\code
const T* tt = new T();
{
  ConstStorageProvider<T> provider(&tt);
}
const T& derefed_tt = *tt;
 */
template <class T>
class ConstStorageProvider
{
public:
  explicit ConstStorageProvider(const T& tt)
    : storage_(make_unique<internal::ConstAccessByReference<T>>(tt))
  {
  }

  /**
   * \attention This ctor transfers ownership to ConstStorageProvider, do not delete tt manually!
   */
  explicit ConstStorageProvider(const T* tt)
    : storage_(make_unique<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::unique_ptr<const T>&& tt)
    : storage_(make_unique<internal::ConstAccessByPointer<T>>(tt))
  {
  }

  explicit ConstStorageProvider(std::shared_ptr<const T> tt)
    : storage_(make_unique<internal::ConstAccessByPointer<T>>(tt))
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
    assert(storage_);
    return storage_->access();
  }

private:
  std::unique_ptr<internal::ConstAccessInterface<T>> storage_;
}; // class ConstStorageProvider

/**
 * \brief Provides generic access to objects of different origins.
 * \sa ConstStorageProvider
 */
template <class T>
class StorageProvider
{
public:
  explicit StorageProvider(T& tt)
    : storage_(make_unique<internal::AccessByReference<T>>(tt))
  {
  }

  /**
   * \attention This ctor transfers ownership to StorageProvider, do not delete tt manually!
   */
  explicit StorageProvider(T* tt)
    : storage_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::unique_ptr<T>&& tt)
    : storage_(make_unique<internal::AccessByPointer<T>>(tt))
  {
  }

  explicit StorageProvider(std::shared_ptr<T> tt)
    : storage_(make_unique<internal::AccessByPointer<T>>(tt))
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
    assert(storage_);
    return storage_->access();
  }

  const T& access() const
  {
    assert(storage_);
    return storage_->access();
  }

private:
  std::unique_ptr<internal::AccessInterface<T>> storage_;
}; // class StorageProvider

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_MEMORY_HH
