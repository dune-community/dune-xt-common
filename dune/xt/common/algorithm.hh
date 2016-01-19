// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014, 2016)
//   Rene Milk       (2014 - 2015)

#ifndef DUNE_XT_COMMON_ALGORITHM_HH
#define DUNE_XT_COMMON_ALGORITHM_HH

#include <utility>
#include <iterator>
#include <algorithm>

#include <dune/xt/common/string.hh>

namespace Dune {
namespace XT {
namespace Common {

//! same as std::copy_if, but with move
template <class InputIt, class OutputIt, class UnaryPredicate>
OutputIt move_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred)
{
  while (first != last) {
    if (pred(*first))
      *d_first++ = std::move(*first);
    first++;
  }
  return d_first;
}

template <class InputIt, class SequenceType = std::vector<std::string>>
SequenceType make_string_sequence(InputIt first, InputIt last)
{
  SequenceType ret;
  const auto to_str = [](const typename std::iterator_traits<InputIt>::value_type& val) { return toString(val); };
  std::transform(first, last, std::back_inserter(ret), to_str);
  return ret;
}

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_ALGORITHM_HH
