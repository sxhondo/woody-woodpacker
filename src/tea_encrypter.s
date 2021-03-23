section   .bss
  k       resd 4
  sum     resb 4
  msg0    resd 1
  msg1    resd 1
  fsize   resd 1

section   .text
  global  tea_encrypt

tea_encrypt:
  push rax
  push rbx
  push rcx

  mov [fsize], rdx                ; save file size

  mov eax, [rsi]
  mov [k], eax

  mov eax, [rsi + 4]
  mov [k + 4], eax

  mov eax, [rsi + 8]
  mov [k + 8], eax

  mov eax, [rsi + 12]
  mov [k + 12], eax               ; save key

  mov ecx, 0                      ; ecx for outer loop iterator

iter_data:
  cmp ecx, [fsize]
  je done

  mov eax, [rdi + 0 * 4]
  mov [msg0], eax                 ; high (uint32_t) of (void *) in msg0
  
  mov eax, [rdi + 1 * 4]
  mov [msg1], eax                 ; low (uint32_t) of (void *) in msg1

  mov eax, 0
  mov [sum], eax
  mov edx, 0                      ; edx for inner loop iterator

high_32bits:
  mov eax, [sum]
  add eax, 0x9E3779B9             ; sum += delta
  mov [sum], eax

  mov eax, [msg1]
  shl eax, 4
  add eax, [k + 0 * 4]

  mov ebx, [msg1]
  add ebx, [sum]
  xor eax, ebx                    ; ((msg1 << 4) + key[0]) ^ (msg1 + sum)

  mov ebx, [msg1]
  shr ebx, 5
  add ebx, [k + 1 * 4]            ; (msg[1] >> 5 + key[1])

  xor eax, ebx
  add [msg0], eax                 ; msg0 += ((msg[1] << 4) + key[0]) ^ (msg[1] + sum) ^ ((msg[1] >> 5) + key[1])

low_32bits:
  mov eax, [msg0]
  shl eax, 4
  add eax, [k + 2 * 4]

  mov ebx, [msg0]
  add ebx, [sum]
  xor eax, ebx                    ; (msg0 << 4) + key[2] ^ (msg0 + sum)

  mov ebx, [msg0]
  shr ebx, 5
  add ebx, [k + 3 * 4]            ; ((msg0 >> 5) + key[3])

  xor eax, ebx
  add [msg1], eax                 ; msg1 += ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3])
   
  inc edx
  cmp edx, 32
  jne high_32bits

write_to_file:
  mov eax, [msg0]
  mov [rdi], eax                  ; write first (uint32_t) bytes

  mov eax, [msg1]
  mov [rdi + 4], eax              ; write second (uint32_t) bytes

  add rdi, 8                      ; move pointer
  inc ecx
  jmp iter_data

done:
  pop rax
  pop rbx
  pop rcx
  ret