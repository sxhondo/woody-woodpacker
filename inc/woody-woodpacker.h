#ifndef _WOODY_WOODPACKER_H_
# define _WOODY_WOODPACKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <elf.h>
#include <sys/mman.h>

#include <time.h>

# define DEBUG          1
# define OUTPUT         "woody"
# define DYN_PAYLOAD    "dyn_tea_decrypter"
# define EXEC_PAYLOAD   "exec_tea_decrypter"

typedef struct    s_woody
{
   uint64_t       target_size;
   Elf64_Addr     target_entry;

   uint64_t       payload_size;
   uint64_t       payload_txt_size;
   Elf64_Off      txt_segment_end;
   Elf64_Off      payload_offset;
   Elf64_Addr     payload_load_address;

   char           is_dyn;
}                 t_woody;

int               mmap_target(char *path, void **dst);
int               mmap_payload(char *path, void **dst);
void              identify_binary(void *target);
Elf64_Off         find_padding_area(void *target);
Elf64_Shdr        *find_section(void *data, char *name);
void              replace_jump_placeholder(void *payload, uint64_t size, uint64_t ep);
void              replace_data_placeholder(void *target, int size, uint64_t ptr);
void              replace_filesize_placeholder(void *target, int size, uint32_t file_size);
void              replace_key_placeholder(void *target, int size, uint32_t key[4]);
uint32_t          *generate_key();

extern void       tea_encrypt(void *msg, const uint32_t key[4], int fsize);

# endif