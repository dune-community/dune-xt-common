// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <dune/stuff/common/logging.hh>

using namespace Dune;
using namespace Dune::Stuff;
using namespace Dune::Stuff::Common;


TEST(TimedPrefixedLogStream, all)
{
  Timer timer;
  TimedPrefixedLogStream out(timer, "prefix", std::cout);
  out << "there should follow a blank line\n\nand some text, followed by std::flush" << std::flush;
  out << "\n\nthis line should be preceded by a blank line and followed by std::endl" << std::endl;
}
