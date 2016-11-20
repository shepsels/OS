# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h> 
# include <sys/time.h>
# include <signal.h>
# include <stdlib.h>
# include <sys/mman.h>

# define MAX_LEN 1024
# define ERROR -1
# define MMAP_FILE "tmp/mmapped.bin"
# define PERM 0600


void sigusr1_handler (int signum);

void sigusr1_handler (int signum)
{
	struct timeval t1, t2;
	struct stat st;
	int mmappedFile, cnt=0, i=0, fileSize;
	double elapsed_microsec;
	char* mapArray;

	// open file
	if((mmappedFile = open(MMAP_FILE, O_RDWR, PERM)) < 0) {
		printf("cannot open mmap file.%s\n", strerror(errno));
		exit(errno);
	}

	// get file size
	if(stat(MMAP_FILE, &st) < 0) {
		printf("Cannot get %s size: %s\n", MMAP_FILE, strerror(errno));
		exit(errno);
	}
	fileSize = st.st_size;

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) { //todo validate it returns negative int on failure
		printf("Cannot start measuring time: %s\n", strerror(errno));
		exit(errno);
	}

	// mapping file
	mapArray = (char*) mmap(NULL,
	                 fileSize,
	                 PROT_READ | PROT_WRITE, 
	                 MAP_SHARED,
	                 mmappedFile,
	                 0);

	if (MAP_FAILED == mapArray) {
		printf("Error mmapping the file: %s\n", strerror(errno));
		close(mmappedFile);
		exit(errno);
	}

	// loop over the array and count 'a' characters until null sign
	while(mapArray[i] == 'a') {
		cnt++;
		i++;
	}

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		close(mmappedFile);	
		exit(errno);
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// print result together with number of bytes written
	printf("%d were read in %f microseconds through mmap\n", cnt+1, elapsed_microsec);

	// close file
	close(mmappedFile);

	// unmap
	if (munmap(mapArray, fileSize) < 0) {
		printf("Cannot unmap file: %s\n", strerror(errno));
		close(mmappedFile);	
		exit(errno);
	}

	// unlink file
	if(unlink(MMAP_FILE) != 0) {
		printf("Cannot delete the file: %s\n", strerror(errno));
		exit(errno);
	}

	exit(0);
}


int main(int argc, char** argv)
{
	// Structure to pass to the registration syscall
	struct sigaction sigusr1, oldSignal, sigign;

	// set signal handlers
	sigusr1.sa_handler = sigusr1_handler;
	sigign.sa_handler = SIG_IGN;

	// Remove any special flag
	sigusr1.sa_flags = 0;
	sigign.sa_flags = 0;

	// Register siguser1 handler
	if (0 != sigaction (SIGUSR1, &sigusr1, NULL))
	{
		printf("Signal handle registration failed. %s\n",strerror(errno));
		return ERROR;
	}
	// register handler to ignore sigterm
	if (0 != sigaction (SIGTERM, &sigign, &oldSignal)) {
		printf("Signal ignoring failed. %s\n",strerror(errno));
		return ERROR;
	}

	// infinite loop
	while(1)
	{
		sleep(2);
	}

	// set back sigterm
	if (0 != sigaction (SIGTERM, &oldSignal, NULL)) {
		printf("Signal restoring failed. %s\n",strerror(errno));
		return ERROR;
	}

	return 0;
}
