#include "woody-woodpacker.h"

extern void tea_encrypt(void *msg, const uint32_t key[4], int fsize);

int
main(int argc, char **argv)
{
   void           *target, *payload;
   int            tsize, psize, p, len, cc_offset;

   if (argc != 3)
   {
      fprintf(stderr, "usage:\n  %s elf_file payload\n", argv[0]);
      exit(1);
   }

   /* Open and map target and payload */
   int tfd = map_target(argv[1], &target, &tsize);
   int pfd = map_payload(argv[2], &payload, &psize);

   inject_payload(target, payload, tsize, psize);
      
   /* encryption */
   uint32_t key[] = {0xAABBFFAA, 0xBBCCFFAA, 0xFFEE22AA, 0xAAEEDDFF};

   // Elf64_Shdr *t_text_sec = find_section(target, ".text");
   // printf("+ Section offset at %lx (%ld bytes) (%ld)\n",
   //    t_text_sec->sh_offset, t_text_sec->sh_size, t_text_sec->sh_size / sizeof(void *));

   // tea_encrypt(target + t_text_sec->sh_offset, key, t_text_sec->sh_size / sizeof(void *));
   
   // insert_key(target + cc_offset, t_text_sec->sh_size, key);
   // insert_file_size(target + cc_offset, t_text_sec->sh_size, t_text_sec->sh_size / sizeof(void *));
   // insert_start_addr(target + cc_offset, p_text_sec->sh_size, t_text_sec->sh_offset + base);

   // t_text_sec = find_section(target, ".comment");
   // printf("+ Section offset at %lx (%ld bytes) (%ld)\n", 
   //    t_text_sec->sh_offset, t_text_sec->sh_size, t_text_sec->sh_size / sizeof(void *));
   // tea_encrypt(target + t_text_sec->sh_offset, key, t_text_sec->sh_size / sizeof(void *));

   munmap(&target, tsize);
   munmap(&payload, psize);
   close(pfd);
   close(tfd);
   return 0;
}