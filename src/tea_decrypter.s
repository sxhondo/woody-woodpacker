section  .data
   k dd 0x0, 0x0, 0x0, 0x0

section   .text
   global  tea_decrypt

tea_decrypt:
   push rax
   push rdi
   push rsi
   push rdx

   mov r11d, edx              ; r11d for fsize

   mov ecx, 0
iter_data:
   cmp ecx, r11d
   je done

   mov r8d, [rdi + 0 * 4]     ; r8 for msg0
   mov r9d, [rdi + 1 * 4]     ; r9 for msg1

   mov edx, 0
   mov r10d, 0xc6ef3720       ; r10 for sum constant

low_32bits:
   mov eax, r8d
   shl eax, 4
   add eax, [k + 2 * 4]

   mov ebx, r8d
   add ebx, r10d
   xor eax, ebx               ; ((msg0 << 4) + key[2]) ^ (msg0 + sum)

   mov ebx, r8d
   shr ebx, 5
   add ebx, [k + 3 * 4]       ; ((msg0 >> 5) + key[3]

   xor eax, ebx
   sub r9d, eax               ; msg1 -= ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3])

high_32bits:
   mov eax, r9d
   shl eax, 4
   add eax, [k + 0 * 4]

   mov ebx, r9d
   add ebx, r10d
   xor eax, ebx               ; ((msg1 << 4) + key[0]) ^ (msg1 + sum)

   mov ebx, r9d
   shr ebx, 5
   add ebx, [k + 1 * 4]       ; ((msg1 >> 5) + key[1]

   xor eax, ebx
   sub r8d, eax               ; msg0 -= ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1])

   sub r10d, 0x9e3779b9       ; sum -= delta;

   inc edx
   cmp edx, 32
   jne low_32bits

write_to_file:
   mov [rdi], r8d
   mov [rdi + 4], r9d
   add rdi, 8
   inc ecx
   jmp iter_data

done:
   pop rdx
   pop rsi
   pop rdi
   pop rax