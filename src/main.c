#include "woody-woodpacker.h"

t_woody           woody;

int
main(int argc, char **argv)
{
   if (argc != 2)
   {
      fprintf(stderr, "usage:\n  %s elf_file\n", argv[0]);
      exit(1);
   }

   int         tfd, pfd;
   void        *target, *payload;

   tfd = mmap_target(argv[1], &target);
   identify_binary(target);

   pfd = woody.is_dyn ? mmap_payload(DYN_PAYLOAD, &payload) 
                           : mmap_payload(EXEC_PAYLOAD, &payload);

   Elf64_Shdr *p_text_sec = find_section(payload, ".text");
   woody.payload_txt_size = p_text_sec->sh_size;

   find_padding_area(target);

   Elf64_Ehdr *ehdr = (Elf64_Ehdr *)target;
   ehdr->e_entry = woody.is_dyn ? woody.payload_offset : woody.payload_load_address;

   if (DEBUG) printf("+ original entry point set to (%#lx)\n", ehdr->e_entry);

   uint32_t    *key = generate_key();
   Elf64_Shdr  *t_txt_sec = find_section(target, ".text");

   replace_jump_placeholder(payload + p_text_sec->sh_offset, 
                           woody.payload_txt_size,
                           woody.target_entry);
   replace_data_placeholder(payload + p_text_sec->sh_offset, 
                           woody.payload_txt_size,
                           t_txt_sec->sh_addr);
   replace_filesize_placeholder(payload + p_text_sec->sh_offset,
                           woody.payload_txt_size,
                           t_txt_sec->sh_size / sizeof(void *));
   replace_key_placeholder(payload + p_text_sec->sh_offset,
                           woody.payload_txt_size,
                           key);

   memmove(target + woody.payload_offset,
            payload + p_text_sec->sh_offset,
            woody.payload_txt_size);

   if (DEBUG) printf("+ encrypting starts at %#lx and lasts (%ld) bytes\n", 
      t_txt_sec->sh_offset, t_txt_sec->sh_size / sizeof(void *));
   tea_encrypt(target + t_txt_sec->sh_offset, 
               key,
               t_txt_sec->sh_size / sizeof(void *));
   // tea_encrypt(target + t_txt_sec->sh_offset, 
   //             key,
   //             5);

   close(tfd);
   close(pfd);
   return 0;
}

Elf64_Off
find_padding_area(void *target)
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
         woody.txt_segment_end = ph_curr->p_offset + ph_curr->p_filesz;
         woody.payload_offset = woody.txt_segment_end;
         woody.payload_load_address = ph_curr->p_vaddr + ph_curr->p_filesz;
         if (DEBUG)
         {
            printf("   * found executable LOAD segment [%d]\n", i);
            printf("   * txt segment ends at %#lx\n", woody.txt_segment_end);
         }
         ph_curr->p_filesz += woody.payload_txt_size;
         ph_curr->p_memsz += woody.payload_txt_size;
      }
      else if (load_found == 1 && ph_curr->p_type == PT_LOAD && (ph_curr->p_flags & PF_R))
      {
         padding_size = ph_curr->p_offset - woody.txt_segment_end;
         if (DEBUG) 
         {
            printf("   * padding area is (%ld) bytes\n", padding_size);
            printf("   * payload txt section is (%ld) bytes\n", woody.payload_txt_size);
         }
         if (padding_size < woody.payload_txt_size)
         {
            fprintf(stderr, "padding area (%ld) is less than payload txt section (%ld)", 
               padding_size, woody.payload_txt_size);
            exit(1);
         }
         break ;
      }
      ph_curr++;
   }
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
   exit(1);
}

void
identify_binary(void *target)
{
   Elf64_Ehdr *t_header = (Elf64_Ehdr *)target;
   if (t_header->e_type == ET_EXEC)
   {
      woody.is_dyn = 0;
      if (DEBUG) printf("+ e_type of binary is ET_EXEC\n");
   }
   else if (t_header->e_type == ET_DYN)
   {
      woody.is_dyn = 1;
      if (DEBUG) printf("+ e_type of binary is ET_DYN\n");
   }
   else
   {
      fprintf(stderr, "- e_type [%x] is not supported\n", t_header->e_type);
      exit(1);
   }

   if (t_header->e_ident[EI_CLASS] == ELFCLASS64)
   {
      if (DEBUG) printf("+ arch of binary is ELF64\n");
   }
   else
   {
      fprintf(stderr, "- x86 is not supported\n");
      exit(1);
   }
   woody.target_entry = t_header->e_entry;
   if (DEBUG) printf("+ original entry point: %#lx\n", woody.target_entry);
}

int
mmap_target(char *path, void **dst)
{
   int fd = open(path, O_RDONLY);
   if (fd < 0)
   {
      perror("open() target file: ");
      exit(1);
   }

   struct stat buf;
   if (fstat(fd, &buf) != 0)
   {
      perror("lstat() target file: ");
      exit(1);
   }
   woody.target_size = buf.st_size;

   void *mem = mmap(NULL, woody.target_size, PROT_READ, MAP_SHARED, fd, 0);
   if (mem == MAP_FAILED)
   {
      perror("mmap() target file: ");
      exit(1);
   }

   int dstfd = open(OUTPUT, O_RDWR | O_CREAT | O_TRUNC, 0777);
   write(dstfd, mem, woody.target_size);
   munmap(mem, woody.target_size);

   void *dstmem = mmap(NULL, woody.target_size, PROT_READ | PROT_WRITE, MAP_SHARED, dstfd, 0);
   if (mem == MAP_FAILED)
   {
      perror("mmap() target file: ");
      exit(1);
   }
   if (DEBUG) printf("+ %s mapped (%ld bytes) at %p\n", path, woody.target_size, dstmem);
   close(fd);
   *dst = dstmem;
   return dstfd;
}

int
mmap_payload(char *path, void **data)
{
   int fd = open(path, O_RDWR);
   if (fd < 0)
   {
      perror("open() payload: ");
      exit(1);
   }

   struct stat buf;
   if (fstat(fd, &buf) != 0)
   {
      perror("lstat() payload file: ");
      exit(1);
   }
   woody.payload_size = buf.st_size;
   

   void *mem = mmap(NULL, woody.payload_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (mem == MAP_FAILED)
   {
      perror("mmap() payload file: ");
      exit(1);
   }
   if (DEBUG) printf("+ %s mapped (%ld bytes) at %p\n", path, woody.payload_size, mem);
   *data = mem;
   return fd;
}

void
replace_data_placeholder(void *target, int size, uint64_t ptr)
{
   uint64_t    ph = 0x1515151515151515;

   if (DEBUG) printf("+ inserting address to decrypt: (%#lx)\n", ptr);
   for (int i = 0; i < size; i++)
   {
      uint64_t substr = *(uint64_t *)(target + i);
      if (substr == ph)
      {
         *(uint64_t *)(target + i) = ptr;
         if (DEBUG) printf("   * found placeholder (%#lx). Replaced with (%#lx)\n", ph, ptr);
         return ;
      }
   }
   fprintf(stderr, "- placeholder not found\n");
   exit(1);
}

void
replace_jump_placeholder(void *payload, uint64_t size, uint64_t ep)
{
   uint32_t     ep_placeholder = 0x11111111;

   if (DEBUG) printf("+ inserting oep (%lx)\n", ep);
   for (int i = 0; i < size; i++)
   {
      uint32_t substr = *(uint32_t *)(payload + i);
      if (substr == ep_placeholder)
      {
         *(uint32_t *)(payload + i) = ep;
         if (DEBUG) printf("   * found placeholder (%#x) in payload file. Replaced with (%#lx)\n", 
            ep_placeholder, ep); 
         return ;
      }
   }
   fprintf(stderr, "- placeholder (%#x) not found\n", ep_placeholder);
   exit(1);
}

void
replace_filesize_placeholder(void *target, int size, uint32_t file_size)
{
   uint32_t    ph = 0x2A2A2A2A;

   if (DEBUG) printf("+ inserting file size to encrypt: (%d)\n", file_size);
   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == ph)
      {
         *(uint32_t *)(target + i) = file_size;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%d)\n", ph, file_size);
         return ;
      }
   }
   fprintf(stderr, "-placeholder not found (%#x)\n", ph);
   exit(1);
}

void
replace_key_placeholder(void *target, int size, uint32_t key[4])
{
   uint32_t    check[4] = {0, 0, 0, 0};
   uint32_t    ph0 = 0x75726976;
   uint32_t    ph1 = 0x73796273;
   uint32_t    ph2 = 0x6e6f6878;
   uint32_t    ph3 = 0x293a6f64;

   if (DEBUG) printf("+ inserting key\n");
   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == ph0)
      {
         *(uint32_t *)(target + i) = key[0];
         check[0] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", ph0, key[0]);
      }
      else if (sub == ph1)
      {
         *(uint32_t *)(target + i) = key[1];
         check[1] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", ph1, key[1]);
      }
      else if (sub == ph2)
      {
         *(uint32_t *)(target + i) = key[2];
         check[2] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", ph2, key[2]);
      }
      else if (sub == ph3)
      {
         *(uint32_t *)(target + i) = key[3];
         check[3] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", ph3, key[3]);
      }
   }
   if (!check[0] || !check[1] || !check[2] || !check[3])
   {
      fprintf(stderr, " some key was not inserted\n");
      exit(1);
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
   for (int i = 0; i < 3; i++) printf("%x", key[i]);
   printf("\n");
   return key;
}