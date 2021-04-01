#ifndef _WOODY_WOODPACKER_H_
# define _WOODY_WOODPACKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <elf.h>
#include <sys/mman.h>

#include <time.h>
#include <limits.h>

# define DEBUG          1
# define OUTPUT         "woody"
# define DYN_PAYLOAD    "dyn_decrypter"
# define EXEC_PAYLOAD   "exec_decrypter"


typedef enum      e_boolean
{
   FALSE,
   TRUE
}                 t_boolean;

typedef struct    s_woody
{
   uint64_t       target_size;
   Elf64_Addr     target_entry;
   Elf64_Off      txt_segment_end;
   Elf64_Off      payload_offset;
   Elf64_Addr     payload_load_address;
   uint64_t       payload_size;
   Elf64_Shdr     *payload_txt_sec;
   uint64_t       payload_txt_size;
   t_boolean      is_dyn;
   uint32_t       *key;
}                 t_woody;

/* mmap_file.c */
int               mmap_target(char *path, void **dst, t_woody *wdy);
int               mmap_payload(char *path, void **dst, t_woody *wdy);

/* woody_woodpacker.c */
Elf64_Shdr       *patch_payload(void *payload, void *target, t_woody *wdy);
Elf64_Shdr        *find_section(void *data, char *name);

/* utils.c */
void              *_memcpy(void *dst, void *src, size_t n);
uint32_t*         generate_key();

/* encrypter.s */
extern void       tea_encrypt(void *msg, const uint32_t key[4], int fsize);

# endif