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
  push r12

  ; print message
  mov rdx, msg_end - msg      
  lea rsi, [rel msg]
  mov rdi, 0x1
  mov rax, 0x1
  syscall

  ; open /proc/<pid>/maps
  xor rsi, rsi
  lea rdi, [rel maps]
  mov rax, 0x2
  syscall

  ; save fd
  mov r10, rax

  ; allocate 16 bytes on stack
  sub sp, 0x10
  xor rbx, rbx

  ; read 16 byte to stack
  mov rax, 0x0
  mov rdi, r10
  lea rsi, [rsp]
  mov rdx, 0x10
  syscall

  mov ecx, 0
parse_byte:
  xor r8, r8
  mov r8b, [rsp]
  cmp r8b, byte 0x2d
  je calculate_offset_done

  cmp r8, 0x39
  jle parse_digit

parse_alpha:
  sub r8, 0x57
  jmp load

parse_digit:
  sub r8, 0x30

load:
  shl rbx, 0x4
  or rbx, r8

  inc cl
  add spl, byte 0x1
  jmp parse_byte

calculate_offset_done:
  sub spl, cl
  add sp, 0x10

  mov r8, rbx
  mov r10, [rel jump]
  add r8, r10
  mov r13, r8                  ; r13 = (mapping + oep)

  ; tea
  mov ecx, 0
  mov rdi, [rel data]          ; rdi = (mapping + txt_start)
  add rdi, rbx
iter_data:
  cmp ecx, [rel fsize]         ; while fsize
  je done

  mov r8d, [rdi + 0 * 4]       ; r8 for msg0
  mov r9d, [rdi + 1 * 4]       ; r9 for msg1

  mov edx, 0
  mov r10d, 0xc6ef3720
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
  inc ecx
  jmp iter_data

done:
  pop r12
  pop r11
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  jmp r13

  ; placeholders 
  msg       db '....WOODY....', 0x0a, 0
  msg_end   db 0x0
  maps      db '/proc/self/maps', 0x0
  jump      dq 0x1111111111111111
  k         dd 0x75726976, 0x73796273, 0x6e6f6878, 0x293a6f64
  fsize     dq 0x2A2A2A2A2A2A2A2A
  data      dq 0x1515151515151515