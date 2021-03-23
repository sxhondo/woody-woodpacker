#include "woody-woodpacker.h"

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
   return NULL;
}

Elf64_Phdr *
find_code_cave(void *data, int *cc_offset, int psize)
{
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)data;
   Elf64_Phdr *curr_seg, *load_seg;

   curr_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr 
         + (unsigned int) elf_hdr->e_phoff);

   for (int i = 0; i < elf_hdr->e_phnum; i++)
   {
      if (curr_seg->p_type == PT_LOAD && curr_seg->p_flags & 0x011)
      {
         printf("   * Found executable LOAD segment (#%d)\n", i);
         load_seg = curr_seg;
         break ;
      }
      curr_seg = (Elf64_Phdr *) ((unsigned char *)curr_seg 
            + (unsigned int) elf_hdr->e_phentsize);         
   }

   int seg_size = 0;
   while (seg_size < curr_seg->p_filesz)
      seg_size += curr_seg->p_align;

   printf("   * Segment size: %d\n", seg_size);

   int count = 0;
   void *t_seg_ptr = data + curr_seg->p_offset;
   for (int i = 0; i < seg_size; i++)
   {
      if (*(char *)t_seg_ptr == 0x00)
      {
         if (count == 0)
            *cc_offset = curr_seg->p_offset + i;
         count++;
         if (count == psize) break;
      }
      else count = 0;
      t_seg_ptr++;
   }
   printf("+ Codecave at offset 0x%x (%d bytes)\n", *cc_offset, count);
   return load_seg;
}

void
patch_payload_to_entry_point(void *target, int ssize, long oep)
{
   long     placeholder = 0x11111111;

   for (int i = 0; i < ssize; i++)
   {
      long substr = *(unsigned int *) (target + i);
      if (substr == placeholder)
      {
         *(unsigned int *)(target + i) = oep; 
         printf("+ Found placeholder (%#lx) in target file. Replaced with (%#lx)\n", placeholder, oep); 
         return ;
      }
   }
   printf("- Placeholder (%#lx) not found\n", placeholder);
}

Elf64_Addr
find_virtual_address(void *data)
{
   Elf64_Ehdr  *elf_hdr = (Elf64_Ehdr *)data;
   Elf64_Phdr  *curr_seg;

   curr_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr 
               + (unsigned int) elf_hdr->e_phoff);

   for (int i = 0; i < elf_hdr->e_shnum; i++)
   {
      if (curr_seg->p_type == PT_LOAD)
      {
         printf("+ Base address (p_vaddr) is %p\n", (void *)curr_seg->p_vaddr);
         return curr_seg->p_vaddr;
      }
      curr_seg = (Elf64_Phdr *) ((unsigned char *)curr_seg 
                  + (unsigned int) elf_hdr->e_phentsize);
   }

   printf("- PT_LOAD segment not found\n");
   return (Elf64_Addr)0x0;
}

int
main(int argc, char **argv)
{
   Elf64_Ehdr     *t_ehdr;
   void           *target, *payload;
   int            tfsize, pfsize, p, len, cc_offset;

   if (argc != 3)
   {
      fprintf(stderr, "Usage:\n  %s elf_file payload\n", argv[0]);
      exit(1);
   }

   /* Open and map target and payload */
   int target_fd = open_duplicate_and_map(argv[1], &target, &tfsize);
   int payload_fd = open_and_map(argv[2], &payload, &pfsize);

   t_ehdr = ((Elf64_Ehdr *)target);
   Elf64_Addr ep = t_ehdr->e_entry;
   printf ("+ Original entry point: %p\n", (void *)ep);

   Elf64_Shdr *p_text_sec = find_section(payload, ".text");
   Elf64_Phdr *t_load_seg = find_code_cave(target, &cc_offset, p_text_sec->sh_size);
   Elf64_Addr base = find_virtual_address(target);

   /* Copy payload in the segment padding area */   
   memmove(target + cc_offset, payload + p_text_sec->sh_offset, 
                           p_text_sec->sh_size);
   t_load_seg->p_filesz += p_text_sec->sh_size;
   t_load_seg->p_memsz += p_text_sec->sh_size;

   /* Patch return address */
   patch_payload_to_entry_point(target + cc_offset, 
                                    p_text_sec->sh_size, (long)ep);

   printf("+ e_entry set to: %p\n", (void *)(base + cc_offset));
   t_ehdr->e_entry = (Elf64_Addr)(base + cc_offset);
   
   close(payload_fd);
   close(target_fd);
   
   return 0;
}