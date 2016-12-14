# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h> 
# include <sys/time.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/mman.h>
# include <signal.h>
# include <limits.h>

# define MAX_LEN 1024
# define ERROR -1
# define PERM 0600
# define FIFOPATH "tmp/osfifo"
# define BUFFER_SIZE 4096


void sigpipe_handler(int signal);

// global variables
double elapsed_microsec;
int writtenBytes, fifoFile;
struct timeval t1, t2;


void sigpipe_handler(int signal) {
	struct sigaction old;

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) {
		printf("Cannot start measuring time: %s\n", strerror(errno));
		exit(errno);
	}

	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// print result together with number of bytes written
	printf("%d were written in %f miliseconds through FIFO\n", writtenBytes, elapsed_microsec);

	// close file
	close (fifoFile);

	// unlink file
	if(unlink(FIFOPATH) != 0) {
		printf("Cannot delete the file: %s\n", strerror(errno));
		exit(errno);
	}

	exit(0);

}

int main(int argc, char** argv)
{
	int NUM, fifoFile, writtenBytes, i, written;
	long lNUM;
	char buffer[BUFFER_SIZE], strtolBuffer[BUFFER_SIZE];

	// sigint structs
	struct sigaction oldSignal, sigign;
	sigign.sa_handler = SIG_IGN;
	sigign.sa_flags = 0;

	// sigpipe struct
	struct sigaction sigpipe;
	sigpipe.sa_handler = sigpipe_handler;
	sigpipe.sa_flags = 0;

	// register handler to ignore sigint
	if (0 != sigaction (SIGINT, &sigign, &oldSignal)) {
		printf("Error, Signal ignoring failed. %s\n",strerror(errno));
		exit(errno);
	}

	// register sigpipe
	if (0 != sigaction (SIGPIPE, &sigpipe, NULL)) {
		printf("Error, Cannot register signal handler for SIGPIPE. %s\n",strerror(errno));
		exit(errno);
	}

	// validate number of arguments
	if(argc != 2) {
		printf("wrong number of arguments.%s\n", strerror(errno));
		exit(errno);
	}

	// read arguments
	lNUM = strtol(argv[1], NULL ,10);
	if (lNUM == LONG_MIN || lNUM == LONG_MAX || lNUM < 0 ) {
		printf("Error, Cannot read arguments. %s\n",strerror(errno));
		exit(errno);		
	}
	NUM = (int) lNUM;


	// open file. check if fifo exists
	fifoFile = open(FIFOPATH, O_WRONLY | O_APPEND);
	if (errno == ENOENT) {
		// create pipe
		if(mkfifo(FIFOPATH, PERM) < 0) {
			printf("Cannot create fifo file.%s\n", strerror(errno));
			exit(errno);
		}

		// now open file for writing
		if ((fifoFile = open(FIFOPATH, O_WRONLY)) < 0) {
			printf("Cannot open fifo file.%s\n", strerror(errno));
			unlink(FIFOPATH);
			exit(errno);
		}

	}

	else {
		if (fifoFile > 0) {
			//fifo file exists. give it correct permissions as should in mkfifo
			if (chmod(FIFOPATH, PERM) < 0) {
				printf("Cannot change permissions.%s\n", strerror(errno));
				close(fifoFile);
				unlink(FIFOPATH);
				exit(errno);
			}
		}

		// error with openfile
		else {
			printf("Cannot open fifo file.%s\n", strerror(errno));
			unlink(FIFOPATH);
			exit(errno);
		}

	}


	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) {
		printf("Cannot start measuring time: %s\n", strerror(errno));
		close(fifoFile);
		unlink(FIFOPATH);
		exit(errno);
	}

	// fill buffer with 'a' NUM times if NUM > buffer size
	for (i=0; i<BUFFER_SIZE; i++) {
		buffer[i] = 'a';
	}

	// write to file the whole BUFFER_SIZE amount as long as NUM >= BUFFER_SIZE
	while (writtenBytes < NUM) {
		if ((written = write(fifoFile, buffer + written, NUM - writtenBytes)) < 0) {
			printf("Cannot write to file: %s\n", strerror(errno));
			close(fifoFile);
			unlink(FIFOPATH);
			exit(errno);
		}
		writtenBytes += written;
	}

	// // write to file the whole BUFFER_SIZE amount as long as NUM >= BUFFER_SIZE
	// while (NUM >= BUFFER_SIZE) {
	// 	if ((written = write(fifoFile, buffer, BUFFER_SIZE)) < 0) {
	// 		printf("Cannot write to file: %s\n", strerror(errno));
	// 		close(fifoFile);
	// 		unlink(FIFOPATH);
	// 		exit(errno);
	// 	}
	// 	writtenBytes += written;
	// 	NUM -= written;
	// }

	// // when NUM < BUFFER_SIZE, need to insert '\0'
	// buffer[NUM] = '\0';

	// // write the rest
	// if ((written = write(fifoFile, buffer, NUM)) < 0) {
	// 	printf("Cannot write to file: %s\n", strerror(errno));
	// 	return ERROR;
	// }
	// writtenBytes+=NUM;

	// Finish time measuring
	if(gettimeofday(&t2, NULL) < 0) {
		printf("Cannot stop time measuring: %s\n", strerror(errno));
		close(fifoFile);
		unlink(FIFOPATH);
		exit(errno);
	}
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// print result together with number of bytes written
	printf("%d were written in %f miliseconds through FIFO\n", writtenBytes, elapsed_microsec);

	// set back sigint
	if (0 != sigaction (SIGINT, &oldSignal, NULL)) {
		printf("Signal restoring failed. %s\n",strerror(errno));
		close(fifoFile);
		unlink(FIFOPATH);
		exit(errno);
	}

	// close file
	close (fifoFile);

	// unlink file
	if(unlink(FIFOPATH) != 0) {
		printf("Cannot delete the file: %s\n", strerror(errno));
		exit(errno);
	}

	// exit
	exit(0);
}
