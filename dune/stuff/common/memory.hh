// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_MEMORY_HH
#define DUNE_STUFF_MEMORY_HH

#include <memory>

namespace Dune {
namespace Stuff {
namespace Common {

//! make_unique implementation via herb sutter: http://herbsutter.com/gotw/_102/
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_MEMORY_HH
