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

// # define OUTPUT_NAME "resources/xsample64"
# define DEBUG 1

/* open.c */
int   map_target(char *fname, void **data, int *fsize);
int   map_payload(char *fname, void **data, int *fsize);
int   get_file_size(int fd);

/* inject.c */
void  inject_payload(void *target, void *payload, int tsize, int psize);


/* encrypt.c */
void  encrypt(void *target, int padoff, Elf64_Addr base);


/* key.c */

# endif