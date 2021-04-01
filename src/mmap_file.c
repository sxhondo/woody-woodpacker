#include "woody-woodpacker.h"

int
mmap_target(char *path, void **data, t_woody *wdy)
{
   int fd = open(path, O_RDWR);
   if (fd < 0)
   {
      perror("open() target: ");
      exit(EXIT_FAILURE);
   }

   struct stat buf;
   if (fstat(fd, &buf) != 0)
   {
      perror("fstat() target: ");
      exit(EXIT_FAILURE);
   }
   wdy->target_size = buf.st_size;

   void *mem = mmap(NULL, wdy->target_size, PROT_READ, MAP_PRIVATE, 
                  fd, 0);
   if (mem == MAP_FAILED)
   {
      perror("mmap() target: ");
      exit(EXIT_FAILURE);
   }

   int dstfd = open(OUTPUT, O_RDWR | O_CREAT, 0777);

   write(dstfd, mem, wdy->target_size);
   munmap(mem, wdy->target_size);

   *data = mmap(NULL, wdy->target_size, PROT_READ | PROT_WRITE, MAP_SHARED, 
                  dstfd, 0);
   if (*data == MAP_FAILED)
   {
      perror("mmap() target: ");
      exit(EXIT_FAILURE);
   }

   if (DEBUG) printf("+ %s mapped (%ld bytes) at %p\n", 
                           path, wdy->target_size, *data);
   close(fd);
   return dstfd;
}

int
mmap_payload(char *path, void **data, t_woody *wdy)
{
   int fd = open(path, O_RDWR);
   if (fd < 0)
   {
      perror("open() payload: ");
      exit(EXIT_FAILURE);
   }

   struct stat buf;
   if (fstat(fd, &buf) != 0)
   {
      perror("lstat() payload file: ");
      exit(EXIT_FAILURE);
   }
   wdy->payload_size = buf.st_size;
   
   *data = mmap(NULL, wdy->payload_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                     fd, 0);
   if (*data == MAP_FAILED)
   {
      perror("mmap() payload file: ");
      exit(EXIT_FAILURE);
   }
   if (DEBUG) printf("+ %s mapped (%ld bytes) at %p\n", 
                           path, wdy->payload_size, *data);
   return fd;
}