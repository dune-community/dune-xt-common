#ifndef DUNE_STUFF_SIGNALS
#define DUNE_STUFF_SIGNALS

#include <signal.h>

#include <dune/stuff/logging.hh>

namespace Stuff {
void handleInterrupt(int signal)
{
  Logger().Info() << "forcefully terminated at " << Logging::TimeString() << std::endl;
  Logger().Flush();
  exit(signal);
}

void installSignalHandlers()
{
  struct sigaction new_action;

  /* Set up the structure to specify the new action. */
  new_action.sa_handler = handleInterrupt;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;

  sigaction(SIGINT, &new_action, NULL);
}
} // end namepsace stuff
#endif
