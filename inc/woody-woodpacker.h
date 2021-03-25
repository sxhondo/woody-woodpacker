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


/* insert.c */
void  insert_key(void *target, int size, uint32_t key[4]);
void  insert_file_size(void *target, int size, uint32_t file_size);
void  insert_start_addr(void *target, int size, unsigned long ptr);
void  insert_jmp_to_ep(void *target, int ssize, long oep);


# endif