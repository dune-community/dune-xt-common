// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_CRTP_HH
#define DUNE_STUFF_COMMON_CRTP_HH

#include <atomic>

#include "exceptions.hh"

#ifdef CHECK_CRTP
#undef CHECK_CRTP
#endif
#ifdef CHECK_AND_CALL_CRTP
#undef CHECK_AND_CALL_CRTP
#endif

#ifdef NDEBUG
#define CHECK_CRTP(dummy)
#else
/**
  * This macro is essentially a thread safe variant of the CHECK_INTERFACE_IMPLEMENTATION macro from dune-common.
  */
#define CHECK_CRTP(__interface_method_to_call__)                                                                       \
  {                                                                                                                    \
    static std::atomic<bool> call(false);                                                                              \
    if (call)                                                                                                          \
      DUNE_THROW_COLORFULLY(Dune::Stuff::Exceptions::CRTP_check_failed,                                                \
                            "The derived class does not implement the required method!");                              \
    call = true;                                                                                                       \
    try {                                                                                                              \
      (__interface_method_to_call__);                                                                                  \
      call = false;                                                                                                    \
    } catch (...) {                                                                                                    \
      call = false;                                                                                                    \
      throw;                                                                                                           \
    }                                                                                                                  \
  }
// CHECK_CRTP
#endif

#ifdef NDEBUG
#define CHECK_AND_CALL_CRTP(__interface_method_to_call__) (__interface_method_to_call__)
#else
/**
  * This macro is essentially a slightly modified copy of the CHECK_AND_CALL_INTERFACE_IMPLEMENTATION macro.
  */
#define CHECK_AND_CALL_CRTP(__interface_method_to_call__) CHECK_CRTP(__interface_method_to_call__)
#endif

namespace Dune {
namespace Stuff {


template <class InterfaceType, class Traits>
class CRTPInterface
{
public:
  typedef typename Traits::derived_type derived_type;

  static inline derived_type& as_imp(InterfaceType& that)
  {
    return static_cast<derived_type&>(that);
  }

  static inline const derived_type& as_imp(const InterfaceType& that)
  {
    return static_cast<const derived_type&>(that);
  }
}; // CRTPInterface


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_CRTP_HH
