// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "signals.hh"

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
namespace Common {

//! reset given signal to default handler
void resetSignal(int signal)
{
  struct sigaction new_action;

  new_action.sa_handler = SIG_DFL;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction(signal, &new_action, NULL);
} // resetSignal

//! example signal handler
void handleInterrupt(int signal)
{
  DSC_LOG_INFO << "forcefully terminated at " << stringFromTime() << std::endl;
  // reset signal handler and commit suicide
  resetSignal(signal);
  kill(getpid(), signal);
} // handleInterrupt

//! type of handler functions
typedef void handler_type(int);

//! calling this from your main() will install handler as callback when signal is received
void installSignalHandler(int signal, handler_type handler)
{
  struct sigaction new_action;

  /* Set up the structure to specify the new action. */
  new_action.sa_handler = handler;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;

  sigaction(signal, &new_action, NULL);
} // installSignalHandler

} // namepsace Common
} // namepsace Stuff
} // namepsace Dune
