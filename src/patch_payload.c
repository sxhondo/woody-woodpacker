#include "woody-woodpacker.h"

# define JMP_PH   0x1111111111111111   /* Address to jump after payload */
# define TXT_PH   0x1515151515151515   /* Address where encrypted area starts */
# define SIZE_PH  0x2A2A2A2A2A2A2A2A   /* Size of encrypted area */
# define K0       0x75726976           /* First 32-bits of key */
# define K1       0x73796273           /* Second 32-bits of key */
# define K2       0x6e6f6878           /* Third 32-bits of key */
# define K3       0x293a6f64           /* Fourth 32-bits of key */

void
replace_key_placeholder(void *target, int size, uint32_t key[4])
{
   uint32_t    check[4] = {0, 0, 0, 0};

   if (DEBUG) printf("+ inserting key\n");
   for (int i = 0; i < size; i++)
   {
      uint32_t sub = *(uint32_t *)(target + i);
      if (sub == K0)
      {
         *(uint32_t *)(target + i) = key[0];
         check[0] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", K0, key[0]);
      }
      else if (sub == K1)
      {
         *(uint32_t *)(target + i) = key[1];
         check[1] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", K1, key[1]);
      }
      else if (sub == K2)
      {
         *(uint32_t *)(target + i) = key[2];
         check[2] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", K2, key[2]);
      }
      else if (sub == K3)
      {
         *(uint32_t *)(target + i) = key[3];
         check[3] = 1;
         if (DEBUG) printf("   * found placeholder (%#x). Replaced with (%x)\n", K3, key[3]);
      }
   }
   if (!check[0] || !check[1] || !check[2] || !check[3])
   {
      fprintf(stderr, " some key was not inserted\n");
      exit(EXIT_FAILURE);
   }
}

void
patch_placeholder(void *target, int size, uint64_t src, uint64_t ph)
{
   if (DEBUG) printf("+ inserting: (%ld)\n", src);
   for (int i = 0; i < size; i++)
   {
      uint64_t sub = *(uint64_t *)(target + i);
      if (sub == ph)
      {
         *(uint64_t *)(target + i) = src;
         if (DEBUG) printf("   * found placeholder (%#lx). Replaced with (%ld)\n", ph, src);
         return ;
      }
   }
   fprintf(stderr, "-placeholder not found (%#lx)\n", ph);
   exit(EXIT_FAILURE);
}

Elf64_Shdr*
patch_payload(void *payload, void *target, t_woody *wdy)
{
   Elf64_Shdr  *t_txt_sec = find_section(target, ".text");
   Elf64_Off   p_offset = wdy->payload_txt_sec->sh_offset;
   uint64_t    p_size = wdy->payload_txt_size;
   uint64_t    dcr_size = t_txt_sec->sh_size / sizeof(void *);

   patch_placeholder(payload + p_offset, p_size, wdy->target_entry, JMP_PH);
   patch_placeholder(payload + p_offset, p_size, t_txt_sec->sh_addr, TXT_PH);
   patch_placeholder(payload + p_offset, p_size, dcr_size, SIZE_PH);
   replace_key_placeholder(payload + p_offset, p_size, wdy->key);
   return t_txt_sec;
}