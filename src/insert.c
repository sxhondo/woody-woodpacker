#include "woody-woodpacker.h"

void  insert_key(void *target, int size, uint32_t key[4])
{
   uint32_t    ph0 = 0x75726976;
   uint32_t    ph1 = 0x73796273;
   uint32_t    ph2 = 0x6e6f6878;
   uint32_t    ph3 = 0x293a6f64;

   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == ph0)
      {
         *(uint32_t *)(target + i) = key[0];
         printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph0, key[0]);
      }
      else if (sub == ph1)
      {
         *(uint32_t *)(target + i) = key[1];
         printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph1, key[1]);
      }
      else if (sub == ph2)
      {
         *(uint32_t *)(target + i) = key[2];
         printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph2, key[2]);
      }
      else if (sub == ph3)
      {
         *(uint32_t *)(target + i) = key[3];
         printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph3, key[3]);
      }
   }
}

void  insert_file_size(void *target, int size, uint32_t file_size)
{
   uint32_t    ph = 0x2A2A2A2A;

   printf("+ Inserting file size: (%d)\n", file_size);
   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == ph)
      {
         *(uint32_t *)(target + i) = file_size;
         printf("   *Found placeholder (%#x). Replaced with (%d)\n", ph, file_size);
         return ;
      }
   }
   printf("- Placeholder not found\n");
}

void  insert_start_addr(void *target, int size, unsigned long ptr)
{
   uint32_t    ph = 0x15151515;

   printf("+ Inserting address: (%#lx)\n", ptr);
   for (int i = 0; i < size; i++)
   {
      unsigned long substr = *(unsigned long *)(target + i);
      if (substr == ph)
      {
         *(unsigned long *)(target + i) = ptr;
         printf("   *Found placeholder (%#x). Replaced with (%#lx)\n", ph, ptr);
         return ;
      }
   }
   printf("- Placeholder not found\n");
}

void  insert_jmp_to_ep(void *target, int ssize, long oep)
{
   uint32_t     ep_placeholder = 0x11111111;

   for (int i = 0; i < ssize; i++)
   {
      uint32_t substr = *(uint32_t *)(target + i);
      if (substr == ep_placeholder)
      {
         *(uint32_t *)(target + i) = oep;
         printf("+ Found placeholder (%#x) in target file. Replaced with (%#lx)\n", ep_placeholder, oep); 
         return ;
      }
   }
   printf("- Placeholder (%#x) not found\n", ep_placeholder);
}