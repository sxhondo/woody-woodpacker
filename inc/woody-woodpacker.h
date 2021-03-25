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

# define OUTPUT_NAME "resources/xsample64"


/* open.c */
int   map_target(char *fname, void **data, int *fsize);
int   map_payload(char *fname, void **data, int *fsize);
int   get_file_size(int fd);

/* inject.c */
void  inject_payload(void *target, void *payload, int tsize, int psize);
Elf64_Shdr *find_section(void *data, char *name);

/* encrypt.c */
void  encrypt(void *target, int cc_offset, uint32_t key[4], Elf64_Addr base);

# endif