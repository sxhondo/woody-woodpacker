#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

// extern void tea_encrypt(void *msg, const uint32_t key[4], int fsize);

extern void tea_decrypt(void *msg, const uint32_t key[4], int fsize);

int 
get_file_size(int fd)
{
   struct stat stat;
   fstat(fd, &stat);
   return stat.st_size;
}

static void encrypt(uint32_t* msg, uint32_t *key)
{
  uint32_t msg0 = msg[0], msg1 = msg[1], sum = 0;
  uint32_t delta = 0x9e3779b9;

   for (int i = 0; i < 32; i++) 
   {
     sum += delta;
     msg0 += ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1]);
     msg1 += ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3]);
   }
   msg[0] = msg0; 
   msg[1] = msg1;
}       

static void decrypt(uint32_t* msg, uint32_t *key) {
    uint32_t msg0 = msg[0], msg1 = msg[1], sum = 0xC6EF3720; 
    uint32_t delta = 0x9e3779b9;

    for (int i = 0; i < 32; i++) 
    {
        msg1 -= ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3]);      
        msg0 -= ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1]);
        sum -= delta;
    }
    msg[0] = msg0; 
    msg[1] = msg1;
}

int main()
{
	uint32_t key[] = {0x0, 0x0, 0x0, 0x0};

   int fd = open("resources/data", O_RDWR, 0777);
   int fsize = get_file_size(fd);
   void *data = mmap(0, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   printf("+ File address %p (%d bytes)\n", data, fsize);
   printf("+ Key address %p\n", key);

   int nsize = fsize / sizeof(void *);

   // tea_encrypt(data, key, nsize);

   tea_decrypt(data, key, nsize);

   // for (int i = 0; i < nsize; i++)
   //    encrypt(data + (i * 8), key);

   // for (int i = 0; i < nsize; i++)
   //    decrypt(data + (i * 8), key);
   return 0;
}