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
# define FILESIZE 40960000	//todo check what should be here
# define ERROR -1
# define FIFOPATH "tmp/osfifo"
# define PERM 0600

int main(int argc, char** argv)
{
	int fifoFile, bytesRead;
	double elapsed_microsec;
	// time measurement
	struct timeval t1, t2;

	// sleep before opening file
	sleep(2);

	// open fifo file for reading
	if((fifoFile = open(FIFOPATH, O_RDONLY, PERM)) < 0) {
		printf("cannot open fifo file.%s\n", strerror(errno));
		return ERROR;
	}

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) {
		printf("Cannot start measuring time: %s\n", strerror(errno));
		return ERROR;
	}

	// read bytes //maybe using read()//todo loop
	//todo implement-*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*-

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		return ERROR;
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;
	
	// print result together with number of bytes written
	printf("%d were read in %f microseconds through fifo\n", bytesRead, elapsed_microsec); //todo edit to fit demands

	// exit
	exit(EXIT_SUCCESS);

}