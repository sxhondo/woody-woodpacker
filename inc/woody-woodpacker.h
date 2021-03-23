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

int 
open_duplicate_and_map(char *fname, void **data, int *fsize);
int
open_and_map(char *fname, void **data, int *fsize);
int 
get_file_size(int fd);

# endif