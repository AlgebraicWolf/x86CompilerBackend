SEGMENT .text
GLOBAL _start

_start:
    call listing2
    mov eax, 1
    xor ebx, ebx
    int 80h

listing0:
    push EBP
    mov EBP, 0
    mov ESI, 0
    mov EDI, 10
    sub ESP, 4
    mov EBX, 0
    mov ECX, ESP
.label0:
    mov EAX, 3
    mov EDX, 1
    int 128
    mov EAX, [ESP+0]
    cmp EAX, 10
    je .label2
    cmp EAX, 45
    jne .label1
    mov EBP, 1
    jmp .label0
.label1:
    mov EAX, ESI
    imul EDI
    mov ESI, [ESP+0]
    sub ESI, 48
    add ESI, EAX
    jmp .label0
.label2:
    cmp EBP, 0
    je .label3
    neg ESI
.label3:
    add ESP, 4
    pop EBP
    mov EAX, ESI
    ret


listing1:
    mov ESI, EAX
    mov EDX, 0
    mov ECX, ESP
    dec ESP
    mov byte [ESP+0], 10
    cmp EAX, EDX
    jge .label0
    neg EAX
.label0:
    mov EBX, 10
    idiv EBX
    add EDX, 48
    dec ESP
    mov [ESP+0], DL
    mov EDX, 0
    cmp EAX, EDX
    jne .label0
    cmp ESI, EDX
    jge .label1
    dec ESP
    mov byte [ESP+0], 45
.label1:
    mov EDX, ECX
    sub EDX, ESP
    mov EAX, 4
    mov EBX, 1
    mov ECX, ESP
    int 128
    add ESP, EDX
    ret


listing2:
    push EBP
    mov EBP, ESP
    sub ESP, 4
    call listing0
    mov [EBP-4], EAX
    mov EAX, [EBP-4]
    push EAX
    call listing3
    add ESP, 4
    mov [EBP-4], EAX
    mov EAX, [EBP-4]
    call listing1
    mov ESP, EBP
    pop EBP
    ret


listing3:
    push EBP
    mov EBP, ESP
    sub ESP, 4
    mov EAX, [EBP+8]
    push EAX
    mov EAX, 0
    pop EBX
    cmp EAX, EBX
    jne .label0
    mov EAX, 1
    mov [EBP+8], EAX
    mov EAX, [EBP+8]
    mov ESP, EBP
    pop EBP
    ret
.label0:
    mov EAX, [EBP+8]
    push EAX
    mov EAX, 0
    pop EBX
    cmp EAX, EBX
    jge .label1
    mov EAX, 1
    push EAX
    mov EAX, [EBP+8]
    pop EBX
    sub EAX, EBX
    mov [EBP-4], EAX
    mov EAX, [EBP-4]
    push EAX
    call listing3
    add ESP, 4
    mov [EBP-4], EAX
    mov EAX, [EBP-4]
    push EAX
    mov EAX, [EBP+8]
    pop EBX
    imul EBX
    mov [EBP+8], EAX
    mov EAX, [EBP+8]
    mov ESP, EBP
    pop EBP
    ret
.label1:
    mov ESP, EBP
    pop EBP
    ret


