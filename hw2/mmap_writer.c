# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h> 
# include <sys/time.h>

# define MAX_LEN 1024
# define FILESIZE 4000	//todo check what should be here
# define ERROR -1
# define TEMPDIR "/tmp"
# define MMAP_FILE "mmapped.bin"


int main(int argc, char** argv)
{
	//nums
	int NUM, RPID;
	double elapsed_microsec;
	// chars
	char mmapedFullname[MAX_LEN];
	char* mapArray;
	// files
	FILE *mmappedFile;
	// time measurement
	struct timeval t1, t2;

	// validate number of arguments
	if(argc != 2) {
		printf("wrong number of arguments.%s\n", strerror(errno));
		return ERROR;
	}

	// read arguments
	NUM = argv[0];
	RPID = argv[1];

	// create path to mmapped file //todo maybe can pass this and create it in def section
	sprintf(mmapedFullname, "%s/%s", TEMPDIR, MMAP_FILE);

	// create mmapped file
	if((mmappedFile = open(mmapedFullname, O_RDWR | O_CREAT, 0600)) < 0) {
		printf("cannot create mmap file.%s\n", strerror(errno));
		return ERROR;
	}
	// todo validate if should do
	// credit: example in moodle
	// Force the file to be of the same size as the (mmapped) array
	if (lseek(mmappedFile, FILESIZE-1, SEEK_SET) == -1) {
		printf("Error calling lseek() to 'stretch' the file: %s\n", strerror(errno));
		return Error;
	}

	// Something has to be written at the end of the file,
	// so the file actually has the new size. 
	result = ;
	if (write(mmappedFile, "", 1) != 1)
	{
		printf("Error writing last byte of the file: %s\n", strerror(errno));
		return Error;
	}

	// mapping file
	mapArray = (char*) mmap(NULL,
	                 FILESIZE,
	                 PROT_READ | PROT_WRITE, 
	                 MAP_SHARED,
	                 mmappedFile,
	                 0);

	if (MAP_FAILED == mapArray)
	{
	printf("Error mmapping the file: %s\n", strerror(errno));
	return Error;
	}

	// start time measurement
	if(gettimeofday(&t1, NULL) < 0) { //todo validate it returns negative int on failure
		printf("Cannot start measuring time: %s\n", strerror(errno));
		return Error;
	}

	//-----------------------------------------------------
	// fill array with sequential 'a'
	for(int i=0, i<NUM-1, i++) {
		mapArray[i] = 'a';
	}
	mapArray[NUM-1] = '\0';

	// send signal to end process
	if(kill(RPID, SIGUSR1) != 0) {
		printf("Cannot terminate reader process: %s. %s\n", RPID, strerror(errno));
	}


	//-----------------------------------------------------

	// Finish time measuring
	gettimeofday(&t2, NULL);
	
	// Counting time elapsed
	elapsed_microsec = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsed_microsec += (t2.tv_usec - t1.tv_usec) / 1000.0;
	
	// print result together with number of bytes written
	printf("%f microseconds passed\n", elapsed_microsec); //todo edit to fit demands

	//close files and free memory todo
}