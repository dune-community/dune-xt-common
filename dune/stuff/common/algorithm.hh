// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_ALGORITHM_HH
#define DUNE_STUFF_ALGORITHM_HH

#include <utility>
#include <iterator>
#include <algorithm>

#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
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
} // namespace Stuff
} // namespace Dune


#endif // DUNE_STUFF_ALGORITHM_HH
