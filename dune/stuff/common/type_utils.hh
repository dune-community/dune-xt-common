// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2015)
//   Rene Milk       (2012 - 2015)

#ifndef DUNE_STUFF_TYPENAMES_HH
#define DUNE_STUFF_TYPENAMES_HH

#include <complex>
#include <memory>
#include <type_traits>

#include <dune/common/version.hh>

// static_assert/AlwaysFalse redirect to avoid warnings
#if DUNE_VERSION_NEWER(DUNE_COMMON, 2, 4) // post 2.3 release
#include <dune/common/typetraits.hh>
#else
#include <dune/common/static_assert.hh>
#endif

#include <dune/stuff/common/color.hh>

/** use this to define Typename specializations in the GLOBAL namespace ONLY **/
#define STUFF_TYPENAME(NAME)                                                                                           \
  namespace Dune {                                                                                                     \
  namespace Stuff {                                                                                                    \
  namespace Common {                                                                                                   \
  template <>                                                                                                          \
  struct Typename<NAME>                                                                                                \
  {                                                                                                                    \
    static const char* value()                                                                                         \
    {                                                                                                                  \
      return #NAME;                                                                                                    \
    }                                                                                                                  \
  };                                                                                                                   \
  }                                                                                                                    \
  }                                                                                                                    \
  }

namespace Dune {
namespace Stuff {
namespace Common {

inline std::string demangleTypename(const std::string& mangled_name)
{
#ifdef __GNUC__
  return abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, nullptr);
#else // ifdef __GNUC__
  return mangled_name;
#endif // ifdef __GNUC__
}

//! demangles typeid
template <class T>
std::string demangledTypeId(T& obj)
{
  return demangleTypename(typeid(obj).name());
} // demangledTypeId

//! create output for demangled typeid
template <class T>
void realTypeId(T& obj, std::string name = "", size_t maxlevel = 10000)
{
  std::cout << name << (name == "" ? "" : "'s type is ") << highlightTemplate(demangledTypeId(obj), maxlevel)
            << std::endl;
}

//! an extensible mechanism to assign "cleartext" names to types
template <typename T>
struct Typename
{
  static std::string value()
  {
#if defined(__GNUC__) && defined(__GXX_RTTI)
    return demangleTypename(typeid(T).name());
#else
    return "unknown";
#endif
  }
};

template <class T>
std::string getTypename(const T&)
{
  return Typename<T>::value();
}

template <class T, class Ptr = void>
struct is_smart_ptr
{
  static const bool value = false;
  typedef T type;
};

template <class T>
struct is_smart_ptr<T, typename std::enable_if<std::is_same<std::unique_ptr<typename T::element_type>, T>::value>::type>
{
  static const bool value = true;
  typedef T type;
};

template <class T>
struct is_smart_ptr<T, typename std::enable_if<std::is_same<std::shared_ptr<typename T::element_type>, T>::value>::type>
{
  static const bool value = true;
  typedef T type;
};

template <class T>
struct is_smart_ptr<T, typename std::enable_if<std::is_same<std::weak_ptr<typename T::element_type>, T>::value>::type>
{
  static const bool value = true;
  typedef T type;
};

template <class T, class = void>
struct PtrCaller
{
  static T& call(T& ptr)
  {
    return ptr;
  }
};

template <class T>
struct PtrCaller<T, typename std::enable_if<is_smart_ptr<T>::value || std::is_pointer<T>::value>::type>
{
  static typename T::element_type& call(T& ptr)
  {
    return *ptr;
  }
};

//! workaround for gcc 4.7 missing underlying type, via
//! https://stackoverflow.com/questions/9343329/how-to-know-underlying-type-of-class-enum/10956467#10956467
template <class T>
struct underlying_type
{
#if __GNUC__ == 4 && (__GNUC_MINOR__ < 7)
  typedef typename std::conditional<T(-1) < T(0), typename std::make_signed<T>::type,
                                    typename std::make_unsigned<T>::type>::type type;
#else
  typedef typename std::underlying_type<T>::type type;
#endif
};

//! gcc < 4.8 fires a static-assert if std::hash< T > () isn't implemented
#if __GNUC__ == 4 && (__GNUC_MINOR__ < 8)
template <typename>
struct is_hashable : std::false_type
{
};
#else
//! implementation from https://gcc.gnu.org/ml/libstdc++/2013-03/msg00027.html
template <typename, typename = void>
struct is_hashable : std::false_type
{
};

template <typename T>
struct is_hashable<T, typename std::enable_if<!!sizeof(std::declval<std::hash<T>>()(std::declval<T>()))>::type>
    : std::true_type
{
};
#endif

} // namespace Common
} // namespace Stuff
} // namespace Dune

/**
  * \brief Helper macro to be used before DSC_has_typedef.
  *
  *        Taken from
  *        http://stackoverflow.com/questions/7834226/detecting-typedef-at-compile-time-template-metaprogramming
  */
#define DSC_has_typedef_initialize_once(tpdef)                                                                         \
  template <typename T_local>                                                                                          \
  struct DSC_has_typedef_helper_##tpdef                                                                                \
  {                                                                                                                    \
    template <typename TT_local>                                                                                       \
    struct void_                                                                                                       \
    {                                                                                                                  \
      typedef void type;                                                                                               \
    };                                                                                                                 \
                                                                                                                       \
    template <typename TT_local, typename = void>                                                                      \
    struct helper                                                                                                      \
    {                                                                                                                  \
      static const bool value = false;                                                                                 \
    };                                                                                                                 \
                                                                                                                       \
    template <typename TT_local>                                                                                       \
    struct helper<TT_local, typename void_<typename TT_local::tpdef>::type>                                            \
    {                                                                                                                  \
      static const bool value = true;                                                                                  \
    };                                                                                                                 \
                                                                                                                       \
    static const bool value = helper<T_local>::value;                                                                  \
  };

/**
  * \brief Macro to statically check a type for a typedef.
  *
  *        To check if a type Foo defines a typedef Bar, put this code somewhere, where a generated helper struct
  *        may be defined (obviously only once for each typedef name!):
\code
DSC_has_typedef_initialize_once(Bar)
\endcode
  *        You can then check for Bar (this will give you a static const bool):
\code
DSC_has_typedef(Bar)< Foo >::value
\endcode
  */
#define DSC_has_typedef(tpdf) DSC_has_typedef_helper_##tpdf

/**
  * \brief Helper macro to be used before DSC_has_static_member.
  *
  *        Taken from http://stackoverflow.com/questions/11927032/sfinae-check-for-static-member-using-decltype
  */
#define DSC_has_static_member_initialize_once(mmbr)                                                                    \
  template <class T_local>                                                                                             \
  struct DSC_has_static_member_helper_##mmbr                                                                           \
  {                                                                                                                    \
    template <class TT_local,                                                                                          \
              class = typename std::enable_if<!std::is_member_pointer<decltype(&TT_local::mmbr)>::value>::type>        \
    static std::true_type helper(int);                                                                                 \
                                                                                                                       \
    template <class>                                                                                                   \
    static std::false_type helper(...);                                                                                \
                                                                                                                       \
    static const bool value = decltype(helper<T_local>(0))::value;                                                     \
  };

/**
  * \brief Macro to statically check a class or struct for a static member.
  *
  *        To check if a class or struct Foo has a static member bar, put this code somewhere, where a generated helper
  *        struct may be defined (obviously only once for each member name!):
\code
DSC_has_static_member_initialize_once(bar)
\endcode
  *        You can then check for bar (this will give you a static const bool):
\code
DSC_has_static_member(bar)< Foo >::value
\endcode
  */
#define DSC_has_static_member(mmbr) DSC_has_static_member_helper_##mmbr

STUFF_TYPENAME(int)
STUFF_TYPENAME(double)
STUFF_TYPENAME(float)
STUFF_TYPENAME(long)
STUFF_TYPENAME(unsigned int)
STUFF_TYPENAME(unsigned long)
STUFF_TYPENAME(char)

namespace Dune {
namespace Stuff {
namespace Common {
namespace internal {

template <class Tt>
struct is_complex_helper
{
  DSC_has_typedef_initialize_once(value_type)

      static const bool is_candidate = DSC_has_typedef(value_type)<Tt>::value;
}; // class is_complex_helper

} // namespace internal

template <class T, bool candidate = internal::is_complex_helper<T>::is_candidate>
struct is_complex : public std::is_base_of<std::complex<typename T::value_type>, T>
{
};

template <class T>
struct is_complex<T, false> : public std::false_type
{
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_TYPENAMES_HH
