#include "woody-woodpacker.h"

void    *_memcpy(void *dst, void *src, size_t n)
{
   size_t  i = 0;
   char  *csrc = (char *)src;
   char  *cdst = (char *)dst;

   if (dst == src)
      return (dst);
   while (i < n)
   {
      cdst[i] = csrc[i];
      i++;
   }
   return (cdst);
}

uint32_t*
generate_key()
{
   static uint32_t key[4] = {0, 0, 0, 0};

   srand(time(NULL));
   key[0] = rand();
   key[1] = rand();
   key[2] = rand();
   key[3] = rand();

   printf("key: 0x");
   for (int i = 0; i < 3; i++) 
      printf("%x", key[i]);
   printf("\n");
   return key;
}