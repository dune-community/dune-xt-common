// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_SIGNALS
#define DUNE_STUFF_SIGNALS

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

#endif // ifndef DUNE_STUFF_SIGNALS
