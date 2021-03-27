#include "woody-woodpacker.h"

int
get_file_size(int fd)
{
   struct stat stat;
   fstat(fd, &stat);
   return stat.st_size;
}

static int
safe_open(char *fname, unsigned int flags, unsigned int mod)
{
   int fd = open(fname, flags, mod);

   if (fd < 0)
   {
      fprintf(stderr, "open() fails\n");
      exit(1);
   }
   return fd;
}

static void
*safe_mmap(int len, int prot, int flags, int fd)
{
   void *data = mmap(0, len, prot, flags, fd, 0);

   if (data == MAP_FAILED)
   {
      fprintf(stderr, "mmap() fails\n");
      exit(1);
   }
   return data;
}

int
map_payload(char *fname, void **data, int *fsize)
{
   int   fd = safe_open(fname, O_RDONLY, 0);
   
   *fsize = get_file_size(fd);
   *data = safe_mmap(*fsize, PROT_READ, MAP_SHARED, fd);
   return fd;
}

int
map_target(char *fname, void **data, int *fsize)
{
   void  *data_src;
   int   srcfd, dstfd, size;

   srcfd = safe_open(fname, O_RDONLY, 0);
   *fsize = get_file_size(srcfd);
   data_src = safe_mmap(*fsize, PROT_READ, MAP_SHARED, srcfd);
   
   if (DEBUG) printf("+ %s mapped (%d bytes) at %p\n", fname, *fsize, data_src);
   
   dstfd = safe_open(OUTPUT_NAME, O_RDWR | O_CREAT | O_TRUNC, 0777);
   write(dstfd, data_src, *fsize);
   if (*fsize != get_file_size(dstfd))
   { 
      fprintf(stderr, "failed to create copy\n"); 
      exit(1); 
   }
   *data = safe_mmap(*fsize, PROT_READ | PROT_WRITE, MAP_SHARED, dstfd);
   if (DEBUG) printf("+ %s mapped (%d bytes) at %p\n", OUTPUT_NAME, *fsize, data);
   munmap(data_src, *fsize);
   close(srcfd);
   return dstfd;
}