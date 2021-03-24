#include "woody-woodpacker.h"

int 
get_file_size(int fd)
{
   struct stat stat;
   fstat(fd, &stat);
   return stat.st_size;
}

int
open_and_map(char *fname, void **data, int *fsize)
{
   int   srcfd, size;

   if ((srcfd = open(fname, O_RDONLY)) < 0)
   {
      perror("open() ");
      exit(1);
   }
   *fsize = get_file_size(srcfd);
   if ((*data = mmap(0, *fsize, PROT_READ, MAP_SHARED, 
         srcfd, 0)) == MAP_FAILED)
   {
      perror("mmap() ");
      exit(1);
   }
   return srcfd;
}

int
open_duplicate_and_map(char *fname, void **data, int *fsize)
{
   void  *mem;
   int   srcfd, dstfd, size;

   /* open original file */
   if ((srcfd = open(fname, O_RDONLY)) < 0)
   {
      perror("open() ");
      exit(1);
   }
   *fsize = get_file_size(srcfd);
   if ((mem = mmap(0, *fsize, PROT_READ, MAP_SHARED, 
         srcfd, 0)) == MAP_FAILED)
   {
      perror("mmap() ");
      exit(1);
   }
   printf("+ %s mapped (%d bytes) at %p\n", fname, *fsize, mem);
   /* make copy */
   char  *dst = "resources/xsample64";
   if ((dstfd = open(dst, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
   {
      perror("open() ");
      exit(1);
   }
   write(dstfd, mem, *fsize);
   if (*fsize != get_file_size(dstfd))
   {
      perror("sizes of src and dst differs");
      exit(1);
   }
   munmap(mem, size);
   close(srcfd);

   /* map copy */
   if ((*data = mmap(0, *fsize, PROT_READ | PROT_WRITE, MAP_SHARED, 
         dstfd, 0)) == MAP_FAILED)
   {
      perror("mmap() ");
      exit(1);
   }
   printf("+ %s mapped (%d bytes) at %p\n", dst, *fsize, data);
   return dstfd;
}