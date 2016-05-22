#include <iostream>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>

#include "Daemon.h"

using namespace std;

bool setDaemon()
{
	int i, fd0, fd1, fd2;
	int pid;
	struct rlimit rl;
	struct sigaction sa;

	// Clear file creation mask.
	umask(0);

	// Get maximum number of file descriptors.
	
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
	{
		cout << "can't get file limit: " << strerror(errno) << endl;
		return false;
	}
	if ((pid = fork()) < 0)
	{
		cout << "can't fork: " << strerror(errno) << endl;
		return false;
	}
	else if (pid != 0)
	{
		// parent 
		exit(0);
	}
	setsid();

	// Ensure future opens won¡ät allocate controlling TTYs.
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
	{
		cout << "can't ignore SIGHUP" << strerror(errno) << endl;
		return false;
	}
	if ((pid = fork()) < 0)
	{
		cout << "can't fork: " << strerror(errno) << endl;
		return false;
	}
	else if (pid != 0)
	{
		exit(0);
	}
		
	// Close all open file descriptors.
	if (rl.rlim_max == RLIM_INFINITY)
	{
		rl.rlim_max = 1024;
	}
	for (i = 0; i < rl.rlim_max; i++)
	{
		close(i);
	}
	
	// Attach file descriptors 0, 1, and 2 to /dev/null.
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	//
	return true;
}