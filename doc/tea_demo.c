#include <stdint.h>

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