#ifndef DUNE_STUFF_TYPENAMES_HH
#define DUNE_STUFF_TYPENAMES_HH

#include "color.hh"

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

std::string demangleTypename(const std::string& mangled_name)
{
#ifdef __GNUC__
  return abi::__cxa_demangle(mangled_name.c_str(), 0, 0, int());
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
void realTypeId(T& obj, std::string name = "", int maxlevel = 10000)
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
} // namespace Common
} // namespace Stuff
} // namespace Dune

STUFF_TYPENAME(int)
STUFF_TYPENAME(double)
STUFF_TYPENAME(float)
STUFF_TYPENAME(long)
STUFF_TYPENAME(unsigned int)
STUFF_TYPENAME(unsigned long)
STUFF_TYPENAME(char)

#endif // DUNE_STUFF_TYPENAMES_HH
