#ifndef DUNE_XT_COMMON_NUMERIC_CAST_HH
#define DUNE_XT_COMMON_NUMERIC_CAST_HH

#include <boost/numeric/conversion/cast.hpp>

#include "exceptions.hh"
#include "type_traits.hh"

namespace Dune {
namespace XT {
namespace Common {


/**
 * \brief Calls boost::numeric_cast and gives some information on failure.
 */
template <class T, class S>
T numeric_cast(S source)
{
  T target;
  try {
    target = boost::numeric_cast<T>(source);
  } catch (boost::bad_numeric_cast& ee) {
    DUNE_THROW(XT::Common::Exceptions::external_error,
               "  The following error occured in boost while converting '" << source << "' to '"
                                                                           << XT::Common::Typename<size_t>::value()
                                                                           << "':\n\n   "
                                                                           << ee.what());
  }
  return target;
} // ... numeric_cast(...)


} // namespace Common
} // namespace XT
} // namespace Dune


#endif // DUNE_XT_COMMON_NUMERIC_CAST_HH
