// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012, 2014)
//   Rene Milk       (2010, 2012 - 2013, 2015)

#ifndef DUNE_XT_COMMON_SIGNALS
#define DUNE_XT_COMMON_SIGNALS

#include <signal.h>

namespace Dune {
namespace Stuff {
namespace Common {

//! POSIX signal management utils

//! reset given signal to default handler
void resetSignal(int signal);

//! example signal handler
void handleInterrupt(int signal);

//! type of handler functions
typedef void handler_type(int);

//! calling this from your main() will install handler as callback when signal is received
void installSignalHandler(int signal = SIGINT, handler_type handler = handleInterrupt);

} // namepsace Common
} // namepsace Stuff
} // namepsace Dune

#endif // DUNE_XT_COMMON_SIGNALS
