section .text
   global _start

_start:
   mov rax, 1
   mov rdi, 1
   lea rsi, [rel msg]
   mov rdx, msg_end - msg
   syscall

   mov rax, 0x11111111
   jmp rax

   mov rax, 60
   mov rdi, 42
   syscall

align 8
   msg db 'This file is infected', 0x0a, 0
   msg_end db 0x0