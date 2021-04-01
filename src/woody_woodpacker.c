#include "woody-woodpacker.h"

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

static void
identify_binary(void *target, t_woody *wdy)
{
   Elf64_Ehdr *t_header = (Elf64_Ehdr *)target;

   if (t_header->e_ident[EI_MAG0] != ELFMAG0 || t_header->e_ident[EI_MAG1] != ELFMAG1 || 
      t_header->e_ident[EI_MAG2] != ELFMAG2 || t_header->e_ident[EI_MAG3] != ELFMAG3)
   {
      fprintf(stderr, "- not an ELF file\n");
      exit(EXIT_FAILURE);
   }

   if (t_header->e_type == ET_EXEC)
   {
      wdy->is_dyn = FALSE;
      if (DEBUG) printf("+ e_type is ET_EXEC\n");
   }
   else if (t_header->e_type == ET_DYN)
   {
      wdy->is_dyn = TRUE;
      if (DEBUG) printf("+ e_type is ET_DYN\n");
   }
   else
   {
      fprintf(stderr, "- e_type [%#x] is not supported\n", t_header->e_type);
      exit(EXIT_FAILURE);
   }

   if (t_header->e_ident[EI_CLASS] == ELFCLASS64)
   {
      if (DEBUG) printf("+ x86_64-bit binary\n");
   }
   else
   {
      fprintf(stderr, "- x86-bit binary is not supported\n");
      exit(EXIT_FAILURE);
   }
   wdy->target_entry = t_header->e_entry;
   if (DEBUG) printf("+ original entry point: %#lx\n", wdy->target_entry);
}

static void
find_padding_area(void *target, t_woody *wdy)
{
   Elf64_Ehdr  *t_header = (Elf64_Ehdr *)target;
   Elf64_Off   ph_offset = t_header->e_phoff;

   Elf64_Phdr  *ph_curr = (Elf64_Phdr *)(target + ph_offset);
   Elf64_Off   padding_size;

   int load_found = 0;
   if (DEBUG) printf("+ looking for padding area\n");
   for (int i = 0; i < t_header->e_phnum; i++)
   {
      if (ph_curr->p_type == PT_LOAD && (ph_curr->p_flags & PF_X))
      {
         load_found = 1;
         ph_curr->p_flags |= PF_W;
         wdy->txt_segment_end = ph_curr->p_offset + ph_curr->p_filesz;
         wdy->payload_offset = wdy->txt_segment_end;
         wdy->payload_load_address = ph_curr->p_vaddr + ph_curr->p_filesz;
         if (DEBUG)
         {
            printf("   * found executable LOAD segment [%d]\n", i);
            printf("   * txt segment ends at %#lx\n", wdy->txt_segment_end);
         }
         ph_curr->p_filesz += wdy->payload_txt_size;
         ph_curr->p_memsz += wdy->payload_txt_size;
      }
      else if (load_found == 1 && ph_curr->p_type == PT_LOAD && (ph_curr->p_flags & PF_R))
      {
         padding_size = ph_curr->p_offset - wdy->txt_segment_end;
         if (DEBUG) 
         {
            printf("   * padding area is (%ld) bytes\n", padding_size);
            printf("   * payload txt section is (%ld) bytes\n", wdy->payload_txt_size);
         }
         if (padding_size < wdy->payload_txt_size || padding_size > INT32_MAX)
         {
            fprintf(stderr, "- padding area (%ld) is less than payload txt section (%ld)\n", 
               padding_size, wdy->payload_txt_size);
            exit(EXIT_FAILURE);
         }
         break ;
      }
      ph_curr++;
   }
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

int
main(int argc, char **argv)
{
   int         tfd, pfd;
   void        *target, *payload;
   Elf64_Ehdr  *ehdr;
   Elf64_Shdr  *shdr;
   t_woody     wdy = { 0 };

   if (argc != 2)
   {
      fprintf(stderr, "usage:\n  %s elf_file\n", argv[0]);
      exit(1);
   }
   tfd = mmap_target(argv[1], &target, &wdy);
   identify_binary(target, &wdy);
   pfd = wdy.is_dyn == TRUE ?
      mmap_payload(DYN_PAYLOAD, &payload, &wdy) 
      : mmap_payload(EXEC_PAYLOAD, &payload, &wdy);

   wdy.payload_txt_sec = find_section(payload, ".text");
   wdy.payload_txt_size = wdy.payload_txt_sec->sh_size;
   find_padding_area(target, &wdy);

   ehdr = (Elf64_Ehdr *)target;
   ehdr->e_entry = wdy.is_dyn == TRUE ? 
      wdy.payload_offset : wdy.payload_load_address;
   if (DEBUG) printf("+ original entry point set to (%#lx)\n", ehdr->e_entry);

   wdy.key = generate_key();
   shdr = patch_payload(payload, target, &wdy);

   tea_encrypt(target + shdr->sh_offset, 
               wdy.key, 
               shdr->sh_size / sizeof(void *));

   memmove(target + wdy.payload_offset, 
            payload + wdy.payload_txt_sec->sh_offset,
            wdy.payload_txt_size);

   if (DEBUG)
   {
      printf("+ encrypting starts at %#lx and lasts (%ld) bytes\n",
      shdr->sh_offset, shdr->sh_size / sizeof(void *));
   }
   close(tfd);
   close(pfd);
   exit(EXIT_SUCCESS);
}