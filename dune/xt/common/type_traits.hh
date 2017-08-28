// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   Rene Milk       (2012 - 2016)
//   Tobias Leibner  (2016)

#ifndef DUNE_XT_COMMON_TYPE_TRAITS_HH
#define DUNE_XT_COMMON_TYPE_TRAITS_HH

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <complex>
#include <memory>
#include <type_traits>
#include <iostream>

#include <dune/common/typetraits.hh>
#include <dune/common/bigunsignedint.hh>


/** use this to define Typename specializations in the GLOBAL namespace ONLY **/
#define DUNE_XT_COMMON_TYPENAME(NAME)                                                                                  \
  namespace Dune {                                                                                                     \
  namespace XT {                                                                                                       \
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
namespace XT {
namespace Common {


inline std::string demangle_typename(const std::string& mangled_name)
{
#ifdef __GNUC__
  return abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, nullptr);
#else // ifdef __GNUC__
  return mangled_name;
#endif // ifdef __GNUC__
}


//! demangles typeid
template <class T>
std::string demangled_type_id(T& obj)
{
  return demangle_typename(typeid(obj).name());
} // demangled_type_id


//! create output for demangled typeid
template <class T>
void real_type_id(T& obj, std::string name = "", size_t maxlevel = 10000)
{
  std::cout << name << (name == "" ? "" : "'s type is ") << highlight_template(demangled_type_id(obj), maxlevel)
            << std::endl;
}


//! an extensible mechanism to assign "cleartext" names to types
template <typename T>
struct Typename
{
  static std::string value()
  {
#if defined(__GNUC__) && defined(__GXX_RTTI)
    return demangle_typename(typeid(T).name());
#else
    return "unknown";
#endif
  }
};

template <class T>
std::string get_typename(const T&)
{
  return Typename<T>::value();
}

template <class T>
std::string get_template_basename(const T&)
{
  const auto str = Typename<T>::value();
  const auto r = str.find("<");
  return str.substr(0, r);
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
  typedef typename std::conditional<T(-1) < T(0),
                                    typename std::make_signed<T>::type,
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


template <class T>
struct is_complex : public std::false_type
{
};

template <class T>
struct is_complex<std::complex<T>> : public std::true_type
{
};

template <class T>
struct is_arithmetic : public std::is_arithmetic<T>
{
};

template <int k>
struct is_arithmetic<Dune::bigunsignedint<k>> : public std::true_type
{
};

/**
 * To be used e.g. with AlwaysFalse:
\code
enum class Switch
{
case_1,
case_2,
};

template <Switch important_switch>
struct do_something
{
  static_assert(AlwaysFalse<typename dependent<Switch>::_typename<important_switch>::type>::value,
                "Not Implemented for this case!");
};

template <>
struct do_something<Switch::case_1>
{
  // ...
};
\endcode
 */
template <typename T>
struct dependent
{
  template <T S>
  struct _typename
  {
    typedef void type;
  };
};


} // namespace Common
} // namespace XT
} // namespace Dune


/**
  * \brief Helper macro to be used before DXTC_has_typedef.
  * \see   DXTC_has_typedef
  *
  *        Taken from
  *        http://stackoverflow.com/questions/7834226/detecting-typedef-at-compile-time-template-metaprogramming
  */
#define DXTC_has_typedef_initialize_once(tpdef)                                                                        \
  template <typename T_local>                                                                                          \
  struct DXTC_has_typedef_helper_##tpdef                                                                               \
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
  }

/**
  * \brief Macro to statically check a type for a typedef.
  *
  *        To check if a type Foo defines a typedef Bar, put this code somewhere, where a generated helper struct
  *        may be defined (obviously only once for each typedef name!, note the trailing `;`!):
\code
DXTC_has_typedef_initialize_once(Bar);
\endcode
  *        You can then check for Bar (this will give you a static const bool):
\code
DXTC_has_typedef(Bar)< Foo >::value
\endcode
  */
#define DXTC_has_typedef(tpdf) DXTC_has_typedef_helper_##tpdf

/**
  * \brief Helper macro to be used before DXTC_has_static_member.
  *
  *        Taken from http://stackoverflow.com/questions/11927032/sfinae-check-for-static-member-using-decltype
  */
#define DXTC_has_static_member_initialize_once(mmbr)                                                                   \
  template <class T_local>                                                                                             \
  struct DXTC_has_static_member_helper_##mmbr                                                                          \
  {                                                                                                                    \
    template <class TT_local,                                                                                          \
              class = typename std::enable_if<!std::is_member_pointer<decltype(&TT_local::mmbr)>::value>::type>        \
    static std::true_type helper(int);                                                                                 \
                                                                                                                       \
    template <class>                                                                                                   \
    static std::false_type helper(...);                                                                                \
                                                                                                                       \
    static const bool value = decltype(helper<T_local>(0))::value;                                                     \
  }

/**
  * \brief Macro to statically check a class or struct for a static member.
  *
  *        To check if a class or struct Foo has a static member bar, put this code somewhere, where a generated helper
  *        struct may be defined (obviously only once for each member name!, note the trailing `;`!):
\code
DXTC_has_static_member_initialize_once(bar);
\endcode
  *        You can then check for bar (this will give you a static const bool):
\code
DXTC_has_static_member(bar)< Foo >::value
\endcode
  */
#define DXTC_has_static_member(mmbr) DXTC_has_static_member_helper_##mmbr


DUNE_XT_COMMON_TYPENAME(int)
DUNE_XT_COMMON_TYPENAME(double)
DUNE_XT_COMMON_TYPENAME(float)
DUNE_XT_COMMON_TYPENAME(long)
DUNE_XT_COMMON_TYPENAME(unsigned int)
DUNE_XT_COMMON_TYPENAME(unsigned long)
DUNE_XT_COMMON_TYPENAME(char)


#endif // DUNE_XT_COMMON_TYPE_TRAITS_HH
