#include "woody-woodpacker.h"

static void
insert_key(void *target, int size, uint32_t key[4])
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
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph0, key[0]);
      }
      else if (sub == ph1)
      {
         *(uint32_t *)(target + i) = key[1];
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph1, key[1]);
      }
      else if (sub == ph2)
      {
         *(uint32_t *)(target + i) = key[2];
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph2, key[2]);
      }
      else if (sub == ph3)
      {
         *(uint32_t *)(target + i) = key[3];
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%x)\n", ph3, key[3]);
      }
   }
}

static void
insert_file_size(void *target, int size, uint32_t file_size)
{
   uint32_t    ph = 0x2A2A2A2A;

   if (DEBUG) printf("+ Inserting file size: (%d)\n", file_size);
   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == ph)
      {
         *(uint32_t *)(target + i) = file_size;
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%d)\n", ph, file_size);
         return ;
      }
   }
   fprintf(stderr, "- Placeholder not found (%#x)\n", ph);
   exit(1);
}

static void
insert_start_addr(void *target, int size, unsigned long ptr)
{
   uint32_t    ph = 0x15151515;

   if (DEBUG) printf("+ Inserting address: (%#lx)\n", ptr);
   for (int i = 0; i < size; i++)
   {
      unsigned long substr = *(unsigned long *)(target + i);
      if (substr == ph)
      {
         *(unsigned long *)(target + i) = ptr;
         if (DEBUG) printf("   *Found placeholder (%#x). Replaced with (%#lx)\n", ph, ptr);
         return ;
      }
   }
   fprintf(stderr, "- Placeholder not found\n");
   exit(1);
}

extern void tea_encrypt(void *msg, const uint32_t key[4], int fsize);

static uint32_t*
generate_key()
{
   static uint32_t key[4];

   srand(time(NULL));
   key[0] = rand();
   key[1] = rand();
   key[2] = rand();
   key[3] = rand();

   printf("key: 0x");
   for (int i = 0; i < 3; i++) printf("%x", key[i]);
   printf("\n");
   return key;
}

void
encrypt(void *target, int padoff, Elf64_Addr base)
{
   uint32_t    *key = generate_key();
   Elf64_Shdr  *sec = find_section(target, ".text");

   if (DEBUG) printf("+ Section offset at %lx (%ld bytes) (%ld)\n",
      sec->sh_offset, sec->sh_size, sec->sh_size / sizeof(void *));
   tea_encrypt(target + sec->sh_offset, key, sec->sh_size / sizeof(void *));
   
   insert_key(target + padoff, sec->sh_size, key);
   insert_file_size(target + padoff, sec->sh_size, sec->sh_size / sizeof(void *));
   insert_start_addr(target + padoff, sec->sh_size, sec->sh_offset + base);

   sec = find_section(target, ".comment");
   if (DEBUG) printf("+ Section offset at %lx (%ld bytes) (%ld)\n", 
      sec->sh_offset, sec->sh_size, sec->sh_size / sizeof(void *));
   tea_encrypt(target + sec->sh_offset, key, sec->sh_size / sizeof(void *));
}