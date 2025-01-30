[bits 32]

global _start

extern fb
extern width
extern height
extern bpp

extern main

_start:
    ; Настройка стека
    mov esp, 0x7c00

    ; Получение и запись информации о VBE
    mov [fb],     eax
    mov [width],  bx
    mov [height], cx
    mov [bpp],    dl

    ; Установка адреса фреймбуфера
    mov edi, [fb]

    ; Установка цвета
    mov eax, 0x0000ff00     ; Зеленый (ARGB)

    ; Расчет общего кол-ва пикселей
    movzx ecx, word [width]
    movzx ebx, word [height]
    imul ecx, ebx

    ; Заливка фреймбуфера
    cld
    rep stosd

    ; Переход в С
    call main

    jmp $
