[bits 16]

global _read_disk

_read_disk:
    push di
    mov di, sp         ; DI указывает на вершину стека
    push es
    push bx
    push si

    mov edx, [di + 6]       ; LBA
    mov ax,  [di + 10]      ; Кол-во секторов для чтения
    mov bx,  [di + 16]      ; Сектор буфера записи
    mov es, bx
    mov bx,  [di + 14]      ; Смещение буфера записи

    ; Создаем DAP (Disk Address Packet) в стеке
    push word 0x0000   ; Резерв (старшие 32 бита LBA)
    push word 0x0000
    push edx           ; Младшие 32 бита LBA
    push es            ; Сегмент буфера
    push bx            ; Смещение буфера
    push ax            ; Количество секторов
    push word 0x0010   ; Размер DAP (16 байт) и зарезервированный байт

    ; Настраиваем параметры для INT 0x13
    mov si, sp         ; SI указывает на DAP в стеке
    mov ah, 0x42       ; Функция расширенного чтения (LBA)
    mov dl,  [di + 18] ; Номер диска
    int 0x13           ; Вызов прерывания
    jc .error          ; Если ошибка (CF=1)

    ; Успех: очищаем стек и возвращаем AX = 0
    add sp, 16         ; Удаляем DAP из стека

    pop si
    pop bx
    pop es
    pop di

    xor ax, ax
    ret
.error:
    add sp, 16         ; Удаляем DAP из стека

    pop si
    pop bx
    pop es
    pop di

    mov ax, 1
    ret
