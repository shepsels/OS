# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h> 
# include <sys/time.h>
# include <stdlib.h>
# include <sys/mman.h>
# include <signal.h>

# define MAX_LEN 1024
# define ERROR -1
# define PERM 0600
# define FIFOPATH "tmp/osfifo"
# define BUFFER_SIZE 4096


void sigpipe_handler(int signal);

// global variables
	double elapsed_microsec;

	// time measurement
	struct timeval t1, t2;




void sigpipe_handler(int signal) {
	struct sigaction old;

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) { //todo validate it returns negative int on failure
		printf("Cannot start measuring time: %s\n", strerror(errno));
		return ERROR;
	}

	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// print result together with number of bytes written
	printf("%d were written in %f microseconds through fifo\n", writtenBytes, elapsed_microsec); //todo edit to fit demands, and make sure writtenBytes holds the required info

	// close file
	close (fifoFile);

	// unlink file
	if(unlink(FIFOPATH) != 0) {
		printf("Cannot delete the file: %s\n", strerror(errno));
		return ERROR;
	}

	exit(0);

}

int main(int argc, char** argv)
{
	int NUM, fifoFile, writtenBytes, i, written;
	char buffer[BUFFER_SIZE];

	// sigint structs
	struct sigaction oldSignal, sigign;
	sigign.sa_handler = SIG_IGN;
	sigign.sa_flags = 0;

	// sigpipe struct
	struct sigaction sigpipe;
	sigpipe.sa_handler = sigpipe_handler;
	sigpipe.sa_flags = 0;


	// set signal handlers

	// register handler to ignore sigint
	if (0 != sigaction (SIGINT, &sigign, &oldSignal)) {
		printf("Signal ignoring failed. %s\n",strerror(errno));
		return ERROR;
	}

	// validate number of arguments
	if(argc != 2) {
		printf("wrong number of arguments.%s\n", strerror(errno));
		return ERROR;
	}

	// read arguments
	NUM  = atoi(argv[1]);

	// create pipe
	if(mkfifo(FIFOPATH, PERM) < 0) {
		printf("Cannot create fifo file.%s\n", strerror(errno));
		return ERROR;
	}


	// open file for writing
	if((fifoFile = open(FIFOPATH, O_WRONLY)) < 0) {
		printf("Cannot open fifo file.%s\n", strerror(errno));
		return ERROR;
	}


	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) { //todo validate it returns negative int on failure
		printf("Cannot start measuring time: %s\n", strerror(errno));
		return ERROR;
	}

	// fill buffer with 'a' NUM times if NUM > buffer size
	for (i=0; i<BUFFER_SIZE; i++) {
		buffer[i] = 'a';
	}

	// write to file the whole BUFFER_SIZE amount as long as NUM >= BUFFER_SIZE
	while (NUM >= BUFFER_SIZE) {
		if ((written = write(fifoFile, buffer, BUFFER_SIZE)) < 0) {
			printf("Cannot write to file: %s\n", strerror(errno));
			return ERROR;
		}
		writtenBytes+=BUFFER_SIZE;
		NUM -= BUFFER_SIZE;
	}

	// when NUM < BUFFER_SIZE, need to insert '\0'
	buffer[NUM] = '\0';

	// write the rest
	if ((written = write(fifoFile, buffer, NUM)) < 0) {
		printf("Cannot write to file: %s\n", strerror(errno));
		return ERROR;
	}
	writtenBytes+=NUM;

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		return ERROR;
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// print result together with number of bytes written
	printf("%d were written in %f microseconds through FIFO\n", writtenBytes, elapsed_microsec); //todo edit to fit demands, and make sure writtenBytes holds the required info

	// close file
	close (fifoFile);

	// unlink file
	if(unlink(FIFOPATH) != 0) {
		printf("Cannot delete the file: %s\n", strerror(errno));
		return ERROR;
	}

	// exit
	return 0;
}
