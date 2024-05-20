section .text
	global _start
_start:
	mov r9, 0
	mov rbx, 1
	mov rcx, 1
	mov r11, 1
metka_1:
	cmp rbx, 5
	jg  metka_2
	mul rbx

	add rbx, 1

	jmp metka_1
metka_2:
	mov r9, 0
metka_3:
	cmp r9, 10000000
	jg  metka_4
	mov rcx, r11
	add r9, 1

	jmp metka_3
metka_4:
	mov rdi, rcx
	mov r12, 1
	call myPrintf
	mov rax, 0x2000001
	xor rdi, rdi
	syscall

%include "myPrintf.s"
