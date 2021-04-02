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

Elf64_Shdr*
find_section(void *data, char *name)
{
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)data;
   Elf64_Shdr *shdr = (Elf64_Shdr *)(data + elf_hdr->e_shoff);
   Elf64_Shdr *sh_strtab = &shdr[elf_hdr->e_shstrndx];
   const char *const sh_strtab_p = data + sh_strtab->sh_offset;

   if (DEBUG) printf("+ looking for section '%s'\n", name);

   for (int i = 0; i < elf_hdr->e_shnum; i++)
   {
      char *sname = (char *)(sh_strtab_p + shdr[i].sh_name);
      if (!strcmp(sname, name)) 
      {
         if (DEBUG) printf("   * .text section found at %#lx (%ld) bytes\n", 
                                             shdr[i].sh_offset, shdr[i].sh_size);
         return &shdr[i];
      }
   }
   fprintf(stderr, "- could not find section'%s'\n", name);
   exit(EXIT_FAILURE);
}