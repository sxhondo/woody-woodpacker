section   .data
  msg     dd 'm', 's', 0    ; double-word (uint32_t) array
  key     dd 0, 0, 0, 0, 0  ; double-word (uint32_t) array

section   .bss
  m1      resb 2

section .text
  global _start ; global tea_encrypt ; void tea_encrypt(uint32_t *msg, uint32_t *key);

_start:
  mov eax, 0x9E3779B9     ; eax for delta
  mov ebx, 0              ; ebx for sum
  mov r8, 0               ; r8 for i
  
  mov ecx, [msg + 1 * 4]  ; ecx for msg[1]
  mov edx, [msg + 0 * 4]  ; edx for msg[0]

iter:
  add ebx, eax            ; sum += delta

  mov esi, ecx
  shl esi, 4
  add esi, [key + 0 * 4]  ; (msg[1] << 4) + key[0]

  mov edi, ecx
  add edi, ebx            ; msg[1] + sum

  xor edi, esi            ; ((msg[1] << 4) + key[0]) ^ (msg1 + sum)

  mov esi, ecx
  shr esi, 5
  add esi, [key + 1 * 4]  ; msg[1] >> 5 + key[1]

  xor edi, esi
  add edx, edi            ; edx += ((msg[1] << 4) + key[0]) ^ (msg[1] + sum) ^ ((msg[1] >> 5) + key[1])

  ; ======

  mov esi, edx
  shl esi, 4
  add esi, [key + 2 * 4]  ; (msg0 << 4) + key[2]

  mov edi, edx
  add edi, ebx            ; msg0 + sum

  xor edi, esi            ; (msg0 << 4) + key[2] ^ (msg0 + sum)

  mov esi, edx
  shr esi, 5
  add esi, [key + 3 * 4]  ; (msg0 >> 5) + key[3]

  xor edi, esi
  add ecx, edi            ; ecx += ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3])

  inc r8
  cmp r8, 32
  jne iter

done:
  mov rax, 60
  xor rdi, rdi
  syscall