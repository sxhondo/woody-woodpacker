section   .text
  global  _start
_start:
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push r11

  mov rax, 1
  mov rdi, 1
  lea rsi, [rel msg]
  mov rdx, msg_end - msg      
  syscall
  
; tea
  mov rcx, 0
  mov rdi, [rel data]          ; get file pointer
iter_data:
  cmp rcx, [rel fsize]         ; while fsize
  je done

  mov r8d, [rdi + 0 * 4]       ; r8 for msg0
  mov r9d, [rdi + 1 * 4]       ; r9 for msg1

  mov edx, 0
  mov r10d, 0xc6ef3720         ; r10 for sum
low_32bits:
  mov eax, r8d
  shl eax, 4
  add eax, [rel k + 2 * 4]

  mov ebx, r8d
  add ebx, r10d
  xor eax, ebx                 ; ((msg0 << 4) + key[2]) ^ (msg0 + sum)

  mov ebx, r8d
  shr ebx, 5
  add ebx, [rel k + 3 * 4]

  xor eax, ebx
  sub r9d, eax                 ; msg1 -= ((msg0 << 4) + key[2]) ^ (msg0 + sum) ^ ((msg0 >> 5) + key[3])
high_32bits:
  mov eax, r9d
  shl eax, 4
  add eax, [rel k + 0 * 4]

  mov ebx, r9d
  add ebx, r10d
  xor eax, ebx                 ; ((msg1 << 4) + key[0]) ^ (msg1 + sum)

  mov ebx, r9d
  shr ebx, 5
  add ebx, [rel k + 1 * 4]

  xor eax, ebx
  sub r8d, eax                 ; msg0 -= ((msg1 << 4) + key[0]) ^ (msg1 + sum) ^ ((msg1 >> 5) + key[1])

  sub r10d, 0x9e3779b9         ; sum -= delta;

  inc edx
  cmp edx, 32
  jne low_32bits
write_to_file:
  mov [rdi], r8d
  mov [rdi + 4], r9d
  add rdi, 8
  inc rcx
  jmp iter_data
done:
  pop r11
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  jmp [rel jump]

; placeholders
msg       db 'wow', 0x0a, 0
msg_end   db 0x0
k         dd 0x75726976, 0x73796273, 0x6e6f6878, 0x293a6f64
fsize     dq 0x2A2A2A2A2A2A2A2A
data      dq 0x1515151515151515
jump      dq 0x1111111111111111