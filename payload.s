section .text
        global _start

_start:
  push rax
  push rdi
  push rsi
  push rdx

  mov rax, 1
  mov rdi, 1
  lea rsi, [rel msg]
  mov rdx, msg_end - msg
  syscall

  pop rdx
  pop rsi
  pop rdi
  pop rax

	mov rax, 0x11111111
	jmp rax

msg     db 'wow', 0x0a, 0
msg_end db 0x0