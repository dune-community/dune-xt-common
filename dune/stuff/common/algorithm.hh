// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_ALGORITHM_HH
#define DUNE_STUFF_ALGORITHM_HH

#include <utility>

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

} // namespace Common
} // namespace Stuff
} // namespace Dune


#endif // DUNE_STUFF_ALGORITHM_HH
