[bits 16]

init_msg db "Stage 2 entered", 0xD, 0xA, 0

extern main
global stage2_entry

stage2_entry:
    ; Настройка сегментных регистров
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax

    ; Настройка стека
    mov ss, ax
    mov sp, 0xBC00

    ; Начальное сообщение
    mov si, init_msg 
    call print_string

    call main

    jmp $

; Функция вывода нуль-терминированной строки
; в текстовом режиме
; Вход: SI - адрес на начало строки
; Выход: -
print_string:
    pusha
.loop:
    lodsb           ; Загрузка символа из SI в AL
    or al, al       ; Проверка на конец строки
    jz .done

    mov ah, 0x0e    ; Функция вывода символа
    mov bh, 0       ; Номер страницы
    int 0x10

    jmp .loop
.done:
    popa
    ret
