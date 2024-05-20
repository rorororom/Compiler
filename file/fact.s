section .text
	global _start
_start:
	mov rax, 1
	mov rbx, 1
metka_1:
	cmp rbx, 5
	jg  metka_2
	mul rbx
	add rbx, 1
	jmp metka_1
metka_2:
	mov rcx, 0
metka_3:
	cmp rcx, 10000000
	jg  metka_4
	mov r13, rax
	add rcx, 1

	jmp metka_3
metka_4:
	mov rdi, r13
	mov r12, 1
	call myPrintf
	mov rax, 0x2000001
    xor rdi, rdi
    syscall

%include "myPrintf.s"
