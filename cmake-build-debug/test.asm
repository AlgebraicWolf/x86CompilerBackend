SEGMENT .text
GLOBAL _start

_start:
    call listing2
    mov eax, 1
    xor ebx, ebx
    int 80h

listing0:
    ret


listing1:
    mov EDX, 0
    sub ESP, 14
    mov EBX, EAX
    and EBX, 2147483648
    cmp EBX, EDX
    mov ECX, ESP
    je .label0
    mov [ECX+0], 45
    inc ECX
.label0:
    cmp EAX, EDX
    je .label1
    mov EBX, 10
    idiv EBX
    add EDX, 48
    mov [ECX+0], EDX
    inc ECX
    mov EDX, 0
    jmp .label0
.label1:
    sub ECX, ESP
    mov EBX, ECX
    mov EAX, 4
    mov EBX, ESP
    int 128
    add ESP, 14
    ret


listing2:
    push EBP
    mov EBP, ESP
    sub ESP, 4
    call listing0
    mov [EBP+0], EAX
    mov EAX, [EBP+0]
    push EAX
    call listing3
    add ESP, 4
    mov [EBP+0], EAX
    mov EAX, [EBP+0]
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
    mov [EBP+0], EAX
    mov EAX, [EBP+0]
    push EAX
    call listing3
    add ESP, 4
    mov [EBP+0], EAX
    mov EAX, [EBP+0]
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


