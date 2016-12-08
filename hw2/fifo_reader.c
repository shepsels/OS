#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


# define MAX_LEN 1024
# define ERROR -1
# define FIFOPATH "tmp/osfifo"
# define PERM 0600
# define BUFFERSIZE 4096

int main(int argc, char** argv)
{
	int fifoFile, bytesRead, fileSize, i, totalRead=0;
	double elapsed_microsec;
	char buffer[BUFFERSIZE];
	struct stat st;
	// time measurement
	struct timeval t1, t2;

	// sleep before opening file
	sleep(2);

	// open fifo file for reading
	if((fifoFile = open(FIFOPATH, O_RDONLY)) < 0) {
		printf("cannot open fifo file.%s\n", strerror(errno));
		return ERROR;
	}

	// // get file size
	// if(stat(FIFOPATH, &st) < 0) {
	// 	printf("Cannot get %s size: %s\n", FIFOPATH, strerror(errno));
	// 	exit(errno);
	// }
	// fileSize = st.st_size;
	// printf("filesize: %d\n", fileSize);

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) {
		printf("Cannot start measuring time: %s\n", strerror(errno));
		return ERROR;
	}

	// read bytes 
	while((bytesRead = read(fifoFile, buffer, BUFFERSIZE)) > 0) {
		// count total number of 'a' characters
		printf("reading %d bytes to buffer.\n", bytesRead);
		for(i=0; i<bytesRead; i++) {
			if(buffer[i] == 'a') {
				totalRead++;
			}
		}
	}

	if (bytesRead < 0) {
		printf("Cannot read from file: %s\n", strerror(errno));
		return ERROR;
	}

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		return ERROR;
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;
	
	// print result together with number of bytes written
	printf("%d were read in %f microseconds through fifo\n", totalRead, elapsed_microsec); //todo edit to fit demands

	// close file
	close(fifoFile);

	// exit
	exit(0);

}