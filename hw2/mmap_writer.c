# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <errno.h> 
# include <sys/mman.h>
# include <signal.h>
# include <limits.h>

# define MAX_LEN 1024
# define ERROR -1
# define TEMPDIR "tmp"
# define MMAP_FILE "mmapped.bin"
# define PERM 0600


int main(int argc, char** argv)
{
	//nums
	int NUM, RPID, mmappedFile, i;
	double elapsed_microsec;
	long lNUM, lRPID;
	// chars
	char mmapedFullname[MAX_LEN];
	char* mapArray;
	// time measurement
	struct timeval t1, t2;
	// signal handlers
	struct sigaction oldSignal, sigign;

	// set signal handlers
	sigign.sa_handler = SIG_IGN;

	// Remove any special flag
	sigign.sa_flags = 0;

	// register handler to ignore sigterm
	if (0 != sigaction (SIGTERM, &sigign, &oldSignal)) {
		printf("Signal ignoring failed. %s\n",strerror(errno));
		exit(errno);
	}

	// validate number of arguments
	if(argc != 3) {
		printf("wrong number of arguments.%s\n", strerror(errno));
		exit(errno);
	}

	// read arguments
	lNUM = strtol(argv[1], NULL ,10);
	if (lNUM == LONG_MIN || lNUM == LONG_MAX || lNUM < 0 ) {
		printf("Error, Cannot read arguments. %s\n",strerror(errno));
		exit(errno);		
	}

	lRPID = strtol(argv[2], NULL ,10);
	if (lRPID == LONG_MIN || lRPID == LONG_MAX || lRPID < 0 ) {
		printf("Error, Cannot read arguments. %s\n",strerror(errno));
		exit(errno);		
	}

	NUM = (int) lNUM;
	RPID = (int) lRPID;

	// create path to mmapped file
	sprintf(mmapedFullname, "/%s/%s", TEMPDIR, MMAP_FILE);

	// create mmapped file
	if((mmappedFile = open(mmapedFullname, O_RDWR | O_CREAT, PERM)) < 0) {
		printf("cannot create mmap file.%s\n", strerror(errno));
		exit(errno);
	}
	// Force the file to be of the same size as the (mmapped) array
	if (lseek(mmappedFile, NUM-1, SEEK_SET) < 0) {// not file size, number of bytes
		printf("Error calling lseek() to 'stretch' the file: %s\n", strerror(errno));
		close(mmappedFile);
		return errno;
	}

	// write to the end of the file
	if (write(mmappedFile, "", 1) != 1) {
		printf("Error writing last byte of the file: %s\n", strerror(errno));
		close(mmappedFile);
		return errno;
	}

	// mapping file
	mapArray = (char*) mmap(NULL,
	                 NUM,
	                 PROT_READ | PROT_WRITE, 
	                 MAP_SHARED,
	                 mmappedFile,
	                 0);

	if (MAP_FAILED == mapArray)
	{
		printf("Error mmapping the file: %s\n", strerror(errno));
		close(mmappedFile);
		return errno;
	}

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) {
		printf("Cannot start measuring time: %s\n", strerror(errno));
		munmap(mapArray, NUM);
		close(mmappedFile);
		return errno;
	}

	// fill array with sequential 'a'
	for (i=0; i<(NUM-1); i++) {
		mapArray[i] = 'a';
	}

	// send signal to end process
	if(kill(RPID, SIGUSR1) != 0) {
		printf("Cannot terminate reader process: %d. %s\n", RPID, strerror(errno));
		munmap(mapArray, NUM);
		close(mmappedFile);
		return errno;
	}

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		munmap(mapArray, NUM);
		close(mmappedFile);
		return errno;
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;
	
	// print result together with number of bytes written
	printf("%d were written in %f miliseconds through mmap\n", (i+1), elapsed_microsec);

	// unmap
	if (munmap(mapArray, NUM) < 0) {
		printf("Cannot unmap file: %s\n", strerror(errno));
		close(mmappedFile);
		return errno;
	}

	// set back sigterm
	if (0 != sigaction (SIGTERM, &oldSignal, NULL)) {
		printf("Signal restoring failed. %s\n",strerror(errno));
		close(mmappedFile);
		return errno;
	}

	// close file and exit
	close(mmappedFile);
	exit(0);
}