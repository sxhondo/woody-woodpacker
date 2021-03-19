#include <stdio.h>
#include <stdint.h>

extern void tea_encrypt(uint32_t msg[static 2], const uint32_t key[static 4]);

int main()
{
   uint32_t msg[2] = {0};
	uint32_t key[4] = {0};

   printf("Original values: %#x : %#x\n", msg[0], msg[1]);
   
   tea_encrypt(msg, key);
   printf("Encrypted: %#x : %#x\n", msg[0], msg[1]);

}