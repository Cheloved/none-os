[bits 16]

; === Текстовые данные === ;
init_msg             db "Stage 2 entered", 0xD, 0xA, 0
disk_ok_msg          db "Kernel loaded from disk", 0xD, 0xA, 0
error_msg            db "Disk error on kernel loading!", 0xD, 0xA, 0

; === Определение GDT === ;
gdt_start:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xffff       ; Лимит (0-15)
    dw 0x0          ; База (0-15)
    db 0x0          ; База (16-23)
    db 0b10011010   ; Present = 1 для используемых сегментов
                    ; Privilege = 00 - "ring"
                    ; Type = 1 - code/data
                    ; Type flags:
                    ;   1 - code
                    ;   0 - conforming
                    ;   1 - readable
                    ;   0 - accessed (managed by CPU)
    db 0b11001111   ; Granularity 1 - limit += 0x1000
                    ; 1 - 32bits
                    ; 00 - для AVL(не используется)
                    ; 1111 - лимит (16-23)
    db 0            ; База (24-31)
gdt_data:
    dw 0xffff       ; Лимит (0-15)
    dw 0x0          ; База (0-15)
    db 0x0          ; База (16-23)
    db 0b10010010   ; Present = 1 для используемых сегментов
                    ; Privilege = 00 - "ring"
                    ; Type = 1 - code/data
                    ; Type flags:
                    ;   0 - data
                    ;   0 - conforming
                    ;   1 - readable
                    ;   0 - accessed (managed by CPU)
    db 0b11001111   ; Granularity 1 - limit += 0x1000
                    ; 1 - 32bits
                    ; 00 - для AVL(не используется)
                    ; 1111 - лимит (16-23)
    db 0            ; База (24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Размер
    dd gdt_start                ; Начало

CODE_SEG equ gdt_code - gdt_start   ; Offset код-дескриптора относительно начала
DATA_SEG equ gdt_data - gdt_start

extern main
extern selected_mode
global stage2_entry

stage2_entry:
    ; Настройка сегментных регистров
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax

    ; Настройка стека
    mov ss, ax
    ; mov sp, 0x7c00
    mov sp, 0xBC00

    ; Начальное сообщение
    mov si, init_msg 
    call print_string

    pusha
    call main
    popa

    mov ax, 0x0000
    int 0x16

    ; Начальное сообщение
    mov si, init_msg 
    call print_string

    ; jmp $

    ; === ПЕРЕХОД В PROTECTED MODE === ;
    ; ; Активация линии А20
    ; mov ax, 0x2401
    ; int 0x15

    ; ; Загрузка GDT
    ; cli
    ; lgdt [gdt_descriptor]

    ; ; Включение защищенного режима
    ; mov eax, cr0
    ; or eax, 0x1
    ; mov cr0, eax

    ; ; Переход в 32-битный режим и обновление CS
    ; jmp CODE_SEG:protected_mode

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

; Функция вывода НЕ-нуль-терминированной строки
; в текстовом режиме
; Вход:
;   - SI: адрес на начало строки
;   - CX: кол-во символов
; Выход: -
print_string_count:
    pusha
.loop:
    lodsb           ; Загрузка символа из SI в AL
    or cx, cx       ; Проверка на конец строки
    jz .done

    mov ah, 0x0e    ; Функция вывода символа
    mov bh, 0       ; Номер страницы
    int 0x10

    loop .loop
.done:
    popa
    ret

; === Обработчики ошибок === ;
disk_error:
    mov si, error_msg
    call print_string
    cli
    hlt

[bits 32]
protected_mode:
    ; Обновление сегментных регистров
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Настройка 32-битного стека
    mov esp, 0x7c00

    ; Проверка адреса фреймбуфера
    ; mov edi, [framebuffer]
    mov edi, [selected_mode+0x07]

    ; Установка цвета
    mov eax, 0x00ff0000     ; Красный (ARGB)

    ; Расчет общего кол-ва пикселей
    ; movzx ecx, word [screen_width]
    ; movzx ebx, word [screen_height]
    movzx ecx, word [selected_mode+0x02]
    movzx ebx, word [selected_mode+0x04]
    imul ecx, ebx

    ; Заливка фреймбуфера
    cld
    rep stosd

    ; Передача информации о VBE режме в ядро
    ; через регистры
    ; mov eax, [framebuffer]
    ; mov bx,  [screen_width]
    ; mov cx,  [screen_height]
    ; mov dl,  [bpp]

    jmp $

