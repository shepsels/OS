#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

//----------------------------------------------------------------------------
// Signal handler.
// Simulate some processing and finish
void my_signal_handler (int signum)
{
  for (int i = 0; i < 3; ++i)
  {
    printf("Processing USR1.\n");
    sleep(2);
  }
  printf("Signal processing is complete\n");
}

//----------------------------------------------------------------------------
// The flow:
// 1. register USR1 signal handler
// 2. go to infinite loop of printing a message every 2 seconds
int main (void)
{
  // Structure to pass to the registration syscall
  struct sigaction new_action;
  // Assign pointer to our handler function
  new_action.sa_handler = my_signal_handler;
  // Remove any special flag
  new_action.sa_flags = 0;
  // Register the handler
  if (0 != sigaction (SIGUSR1, &new_action, NULL))
  {
    printf("Signal handle registration failed. %s\n",strerror(errno));
    return -1;
  }

  // Meditate untill killed
  while(1)
  {
    sleep(2);
    printf("Process runs.\n");
  }
  return 0;
}
//============================= END OF FILE ==================================

