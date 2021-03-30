#include "woody-woodpacker.h"

/*
static void
encrypt(uint32_t* msg, uint32_t *key)
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

static void
decrypt(uint32_t* msg, uint32_t *key) {
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
*/
// extern void tea_encrypt(void *msg, const uint32_t key[4], int fsize);

// extern void tea_decrypt(void *msg, const uint32_t key[4], int fsize);

extern void dyn_tea_decrypter(void *msg, const uint32_t key[4], int fsize);

int
main(int ac, char **av)
{
	uint32_t key[] = {0x0, 0x0, 0x0, 0x0};

   int fd = open("resources/xsample64", O_RDWR, 0777);
   struct stat buf;
   fstat(fd, &buf);
   int fsize = buf.st_size;
   void *data = mmap(0, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   printf("+ File address %p (%d bytes)\n", data, fsize);
   printf("+ Key address %p\n", key);

   dyn_tea_decrypter(data, key, fsize / sizeof(void *));

   // for (int i = 0; i < nsize; i++)
   //    encrypt(data + (i * 8), key);

   // for (int i = 0; i < nsize; i++)
   //    decrypt(data + (i * 8), key);
   return 0;
}