#include "woody-woodpacker.h"

static void encrypt(uint32_t* msg, uint32_t *key)
{
  uint32_t msg0 = msg[0], msg1 = msg[1], sum = 0;
  uint32_t delta = 0x9e3779b9;

   for (int i = 0; i < 32; i++) 
   {
     sum += delta;
     msg0 += ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1]);
     msg1 += ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3]);
   }
   msg[0] = msg0; 
   msg[1] = msg1;
}       

static void decrypt(uint32_t* msg, uint32_t *key) {
    uint32_t msg0 = msg[0], msg1 = msg[1], sum = 0xC6EF3720; 
    uint32_t delta = 0x9e3779b9;

    for (int i = 0; i < 32; i++) 
    {
        msg1 -= ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3]);      
        msg0 -= ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1]);
        sum -= delta;
    }
    msg[0] = msg0; 
    msg[1] = msg1;
}

// extern void tea_encrypt(void *msg, const uint32_t key[4], int fsize);

extern void tea_decrypt(void *msg, const uint32_t key[4], int fsize);

Elf64_Shdr *
find_section(void *data, char *name)
{
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)data;
   Elf64_Shdr *shdr = (Elf64_Shdr *)(data + elf_hdr->e_shoff);
   Elf64_Shdr *sh_strtab = &shdr[elf_hdr->e_shstrndx];
   const char *const sh_strtab_p = data + sh_strtab->sh_offset;

   printf("+ %d section in file. Looking for section '%s'\n",
      elf_hdr->e_shnum, name);

   for (int i = 0; i < elf_hdr->e_shnum; i++)
   {
      char *sname = (char *) (sh_strtab_p + shdr[i].sh_name);
      if (!strcmp(sname, name)) return &shdr[i];
   }
   printf("- Could not find '%s'\n", name);
   return NULL;
}
 //0x401297
int main(int ac, char **av)
{
	uint32_t key[] = {0x0, 0x0, 0x0, 0x0};

   if (ac != 2)
   {
      printf("./decrypter file");
      exit(1);
   }

   int fd = open("resources/xsample64", O_RDWR, 0777);
   int fsize = get_file_size(fd);
   void *data = mmap(0, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   Elf64_Shdr *section = find_section(data, ".comment");

   printf("+ File address %p (%d bytes)\n", data, fsize);
   printf("+ Key address %p\n", key);

   int nsize = fsize / sizeof(void *);

   // tea_encrypt(data, key, nsize);

   tea_decrypt(data + section->sh_offset, key, section->sh_size / sizeof(void *));

   // for (int i = 0; i < nsize; i++)
   //    encrypt(data + (i * 8), key);

   // for (int i = 0; i < nsize; i++)
   //    decrypt(data + (i * 8), key);
   return 0;
}