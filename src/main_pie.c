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
find_code_cave(void *data, int tfsize, int ssize, int *offset)
{
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)data;
   Elf64_Phdr *elf_seg;
   
   elf_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr + (unsigned int) elf_hdr->e_phoff);

   for (int i = 0; i < elf_hdr->e_phnum; i++)
   {
      if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & 0x011)
      {
         printf("+ Found PT_LOAD segment (#%d)\n", i);
         break ;
      }
      elf_seg = (Elf64_Phdr *)((unsigned char *)elf_seg + (unsigned int)elf_hdr->e_phentsize);
   }

   int seg_size = 0;
   while (seg_size < elf_seg->p_filesz)
      seg_size += elf_seg->p_align;
   printf("+ Segment size: %d\n", seg_size);

   int cc_offset, count = 0;
   void *t_seg_ptr = data + elf_seg->p_offset;

   printf("+ Looking for codecave of size: (%d bytes)\n", ssize);
   for (int i = 0; i < seg_size; i++)
   {
      if (*(char *)t_seg_ptr == 0x00)
      {
         if (count == 0)
            cc_offset = elf_seg->p_offset + i;
         count++;
         if (count == ssize) break ;
      }
      else count = 0;
      t_seg_ptr++;
   }
   if (count == 0) printf("+ No sufficiency large codecave found\n");
   printf ("+ .text segment codecave at offset %#x (%d bytes available)\n", cc_offset, count);
   *offset = cc_offset;
   return elf_seg;
}

void
patch_target_entry_point(void *target, int cc_offset)
{
   Elf64_Ehdr *e_hdr = (Elf64_Ehdr *)target;
   Elf64_Phdr *p_hdr = (Elf64_Phdr *)((unsigned char *)e_hdr + (unsigned int)e_hdr->e_phoff); 
   Elf64_Addr *base;

   printf("+ Original entry point is at: %#x\n", (unsigned int)e_hdr->e_entry);
   for (int i = 0; i < e_hdr->e_phnum; i++)
   {
      if (p_hdr->p_type == PT_LOAD)
      {
         base = (Elf64_Addr *)p_hdr->p_vaddr;
         printf("+ Base address is at: %#lx\n", (uint64_t)base);
         break ;
      }
      p_hdr = (Elf64_Phdr *)((unsigned char *)p_hdr + (unsigned int)e_hdr->e_phentsize);
   }

   e_hdr->e_entry = (unsigned long)base + cc_offset;
   printf("+ Target entry point set to: %#lx\n", (uint64_t)e_hdr->e_entry);
}

void
patch_payload_to_entry_point(void *target, int cc_offset, Elf64_Addr oep, int ssize)
{
   long     placeholder = 0x11111111;
   void     *sc_ptr = target + cc_offset;

   for (int i = 0; i < ssize; i++)
   {
      long substr = *(unsigned int *) (sc_ptr + i);
      if (substr == placeholder)
      {
         *(unsigned int *)(sc_ptr + i) = oep; 
         printf("+ Found placeholder (%#lx) in target file. Replaced with (%#lx)\n", placeholder, oep); 
         return ;
      }
   }
}

// void
// augment_s(void *target, long scsize, int cc_offset)
// {
//    Elf64_Ehdr *target_ehdr = (Elf64_Ehdr *)target;
//    Elf64_Phdr *target_phdr = (Elf64_Phdr *)((unsigned char *)target_ehdr 
//                                                 + (unsigned int)target_ehdr->e_phoff);
   
//    target_phdr->p_memsz =+ scsize;
//    Elf64_Shdr *target_schdr = target + target_ehdr->e_shoff;

//    for (int i = 0; i < target_ehdr->e_shnum - 1; i++)
//    {
//       if (target_schdr[i].sh_offset < cc_offset && target_schdr[i + 1].sh_offset > cc_offset)
//          target_schdr[i].sh_size += scsize;
//    }
// }

int
find_symbol (void *data, char*name)
{
  Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) data;
  Elf64_Shdr *shdr = (Elf64_Shdr *)(data + elf_hdr->e_shoff);
  Elf64_Shdr* s;
  Elf64_Sym  *symbol;
  char       *sh_strtab_p, *sh_symtab_p;
  int        n_entries;
  
  if ((s = find_section (data, ".symtab")) == NULL)
   {
      fprintf (stderr, "Cannot find symtable\n");
      return -1;
   }

   n_entries = s->sh_size / s->sh_entsize;
   shdr = (Elf64_Shdr *)(data + elf_hdr->e_shoff);
   sh_strtab_p = (char *)(data + shdr[s->sh_link].sh_offset);
   sh_symtab_p = data + s->sh_offset;

   for (int i = 0; i < n_entries; i++)
   {
      symbol = &((Elf64_Sym *)sh_symtab_p)[i];
      printf("%s\n", sh_strtab_p + symbol->st_name);
      if (symbol->st_name)
	   {
	      if (!strcmp (sh_strtab_p + symbol->st_name, name))
	         return symbol->st_value;
	   }
   }

  return -1;
}

int
main(int argc, char **argv)
{
   Elf64_Ehdr     *t_ehdr;
   void           *target, *payload;
   int            tfsize, pfsize1, p, len, cc_offset;

   if (argc != 3)
   {
      fprintf(stderr, "Usage:\n  %s elf_file payload\n", argv[0]);
      exit(1);
   }

   /* Open and map target and payload */
   int target_fd = open_duplicate_and_map(argv[1], &target, &tfsize);
   int payload_fd = open_and_map(argv[2], &payload, &pfsize1);
   
   t_ehdr = ((Elf64_Ehdr *)target);
   Elf64_Addr ep = t_ehdr->e_entry;

   Elf64_Shdr *p_text_sec = find_section(payload, ".text");
   Elf64_Phdr *t_load_seg = find_code_cave(target, tfsize, p_text_sec->sh_size, &cc_offset);
   Elf64_Addr base = t_load_seg->p_vaddr;

   /* Calculate offset to original entry point */
   Elf64_Addr nep = ((Elf64_Ehdr *)payload)->e_entry;
   printf ("+ Payload Entry point    : %lx\n", nep);

   Elf64_Addr _label = find_symbol(payload, "_label");
   printf("+ _label symbol found at : 0x%lx\n", _label);

   _label += (1 - nep);
   printf ("+ Payload jump offset    : %lx\n", _label);
   printf ("+ Final jump offset      : %lx\n", _label + p);

   nep = - (_label + p - ep) - 4;
   printf ("+ Adjust offset to patch : %x\n", (uint32_t)nep);

   /* Copy payload in the segment padding area */
   memmove(target + cc_offset, payload + p_text_sec->sh_offset, p_text_sec->sh_size);
   
   /* Patch return address */
   memmove(target + cc_offset + _label, &nep, sizeof(uint32_t));

   t_ehdr->e_entry = (Elf64_Addr)(base + cc_offset);
   
   close(payload_fd);
   close(target_fd);
   
   return 0;
}