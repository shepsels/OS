#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

#define FILEPATH "tmp/mmapped.bin"
#define FILESIZE 4000

int main(int argc, char *argv[])
{
  int i;
  int fd;
  int result;
  char *arr;

  // open a file for writing.
  // Note: read/write mode needs to match 
  // the required access in mmap (not intuitive)
  fd = open(FILEPATH, O_RDWR | O_CREAT);
  if (-1 == fd) 
  {
    printf("Error opening file for writing: %s\n", strerror(errno));
    return -1;
  }

  // Force the file to be of the same size as the (mmapped) array
  result = lseek(fd, FILESIZE-1, SEEK_SET);
  if (-1 == result)
  {
    printf("Error calling lseek() to 'stretch' the file: %s\n", strerror(errno));
    return -1;
  }
  printf("seek: %d\n", &SEEK_CUR);

  // Something has to be written at the end of the file,
  // so the file actually has the new size. 
  result = write(fd, "", 1);
  if (1 != result)
  {
    printf("Error writing last byte of the file: %s\n", strerror(errno));
    return -1;
  }

  //Now the file is ready to be mmapped.
  arr = (char*) mmap(NULL,
                     FILESIZE,
                     PROT_READ | PROT_WRITE, 
                     MAP_SHARED,
                     fd,
                     0);

  if (MAP_FAILED == arr)
  {
    printf("Error mmapping the file: %s\n", strerror(errno));
    return -1;
  }

  // now write to the file as if it were memory
  for (i = 0; i <= FILESIZE; ++i) 
  {
    arr[i] = 'a';
    // if we want to be sure that the changes are committed 
    // immediately to the file then we have to perform synchronization
    // at this point. Check "msync" system call (with MS_SYNC)
  }

  // don't forget to free the mmapped memory
  // this also ensures the changes commit to the file
  if (-1 == munmap(arr, FILESIZE)) 
  {
    printf("Error un-mmapping the file: %s\n", strerror(errno));
    return -1;
  }

  // un-mmaping doesn't close the file, so we still need to do that.
  close(fd);
  return 0;
}
