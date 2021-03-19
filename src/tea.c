#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t KEY[4] = {0, 0, 0, 0};

void encrypt(uint32_t* msg)
{
  uint32_t msg0 = msg[0], msg1 = msg[1], sum = 0;
  uint32_t delta = 0x9e3779b9;
  for (int i = 0; i < 32; i++) {
    sum += delta;
    msg0 += ((msg1 << 4) + KEY[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + KEY[1]);
    msg1 += ((msg0 << 4) + KEY[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + KEY[3]);
  }
  msg[0] = msg0; msg[1] = msg1;
  printf("%x %x\n", msg0, msg1);
  exit(0);
}       

void decrypt (uint32_t* v) {
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  // set up 
    uint32_t delta=0x9e3779b9;                     // a key schedule constant 
    for (i=0; i<32; i++) {                         // basic cycle start 
        v1 -= ((v0<<4) + KEY[2]) ^ (v0 + sum) ^ ((v0>>5) + KEY[3]);
        v0 -= ((v1<<4) + KEY[0]) ^ (v1 + sum) ^ ((v1>>5) + KEY[1]);
        sum -= delta;
    }                                              // end cycle 
    v[0]=v0; v[1]=v1;
}

int main()
{
		uint32_t v[] = {'m', 's'};
			
		printf("Original Values: ");
		printf("[ %c %c ]\n", v[0], v[1]);

    encrypt(v);

    printf("\nEncrypted:       ");
    printf("[ %x %x ]\n", v[0], v[1]);
    decrypt(v);
    printf("\nDecrypted:       ");
    printf("[ %c %c ]\n", v[0], v[1]);
    return 0;
}