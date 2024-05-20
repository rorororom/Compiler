myPrintf:
	pop rax
	mov [rel ret_adress], rax
    push rdi

Next:
    call PrintFFF

    push rax
    mov rax, 0x2000004
    mov rdi, 1
    mov rdx, msg_len
    mov rsi, buffer
    syscall

    mov rdi, buffer
    mov rcx, 512
    xor al, al
    rep stosb

	pop rax                             ; rax = len
	pop rbx
    push qword [rel ret_adress]         ; Восстанавливаем адрес возврата
    ret

PrintFFF:
    push rbp

    lea rbx, [rel buffer]

    mov r9, 8

    PrintFLoop:
        xor rax, rax
        cmp r12, 0
        je PrintExit

		dec r12
        jmp TypeInt

        mov byte [rbx + r8], al
        inc r8

        jmp PrintFLoop
    PrintExit:
        pop rbp
        mov byte [rbx+ r8], 10
        inc r8

        mov rax, r8
        ret
TypeInt:
    push r9
    add r9, 16
    mov rax, qword [rsp + r9]

itoa:
    xor rdx, rdx
    xor rcx, rcx
    mov r9, 10
    cmp rax, 0
    jne check_negative
    mov byte [rbx + r8], '0'
    inc r8
    jmp AfterItoaLoop

check_negative:
    test rax, 80000000h
    jz itoa_loop

number_is_negative:
    neg eax
    mov byte [rbx + r8], '-'
    inc r8

itoa_loop:
    test rax, rax
    jz BuffNumLoop

    inc rcx
    xor rdx, rdx
    div r9
    push rdx
    jmp itoa_loop

BuffNumLoop:
    pop rax
    add al, '0'
    mov byte [rbx + r8], al
    inc r8
    dec rcx
    jnz BuffNumLoop

AfterItoaLoop:
    pop r9
    add r9, 8
    inc rdi
    jmp PrintFLoop

End:
    pop rax
    push qword [rel ret_adress]
    ret

section .data
    string_end equ 0x00
    ret_adress dq 0
    ret_a dq 0
    ret_a2 dq 0
    save_rsi dq 0
    buffer times 512 db 0
    msg_len equ $ - buffer
    alphabet db "0123456789ABCDEF"
    remainders_number db 0
