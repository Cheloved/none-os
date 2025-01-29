[bits 16]
[org 0xBE00]

stage2:
    ; Настройка сегментных регистров
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax

    ; Настройка стека
    mov ss, ax
    mov sp, 0x7c00

    ; Начальное сообщение
    mov si, init_msg 
    call print_string

    jmp $

    ; === Переход в графический режим VBE === ;
    ; Загрузка информации в vbe_info
    mov ax, 0x4F00
    mov di, vbe_info
    int 0x10

    ; Проверка на ошибку
    cmp ax, 0x004F
    jne vbe_read_error

     ;Вывод смещения и сегмента
    ; mov si, vbe_seg_msg
    ; call print_string
    ; mov dx, [vbe_info+0x0e]
    ; call print_hex_16
    ; mov dx, [vbe_info+0x10]
    ; call print_hex_16

    ; Поиск подходящего режима
    mov si, [vbe_info+0x10]     ; Сегмент списка
    mov fs, si             
    mov si, [vbe_info+0x0E]     ; Смещение списка

mode_loop:
    mov cx, [fs:si]     ; Чтение номера режима
    cmp cx, 0xFFFF      ; 0xFFFF обозначает конец списка режимов
    je loop_not_found
    add si, 2           ; Переход к следующему режиму для след итерации

    ; Получение информации о режиме в vbe_mode_info
    mov ax, 0x4F01
    mov di, vbe_mode_info
    int 0x10
    cmp ax, 0x004F      ; Неподдерживаемый режим
    jne mode_loop

    ; Проверка атрибутов режима
    test word [vbe_mode_info+0x00], 0x80    ; Если 1, использует линейный буфер
    jz mode_loop                            ; Если нет, переход к следующему режиму

    mov eax, [vbe_mode_info+0x28]
    cmp eax, 0
    je mode_loop     ; Пропустить, если адрес нулевой

    ; cmp word [vbe_mode_info+0x12], 0d1920   ; Ширина = 1024 пикселя
    cmp word [vbe_mode_info+0x12], 0d640   ; Ширина = 1024 пикселя
    jne mode_loop

    cmp byte [vbe_mode_info+0x19], 0d32     ; 32 бита на пиксель
    jne mode_loop

    ; Если найден, установить режим
    ;mov si, mode_found_msg
    ;call print_string
    ;mov dx, cx          ; Вывод номера режима
    ;call print_hex_16

    ;mov si, fb_msg
    ;call print_string
    ;mov dx, [vbe_mode_info+0x28]  ; Вывод начала буффера
    ;call print_hex_16
    ;mov dx, [vbe_mode_info+0x2A]  ; Вывод начала буффера
    ;call print_hex_16
    ;
    ;mov dx, [vbe_mode_info+0x12]  ; Ширина
    ;call print_hex_16
    ;mov dx, [vbe_mode_info+0x14]  ; Высота
    ;call print_hex_16
    ;xor dx, dx
    ;mov dl, [vbe_mode_info+0x19]  ; Высота
    ;call print_hex_16

    ;jmp $

    mov ax, 0x4F02
    mov bx, cx
    or bx, 0x4000   ; Использовать линейный буфер
    int 0x10

    cmp ax, 0x004F
    jne vbe_set_error

    ; Сохранение параметров VBE
    mov eax, [vbe_mode_info+0x28]   ; Адрес начала буфера
    mov [framebuffer], eax          

    mov ax, [vbe_mode_info+0x12]    ; Ширина экрана
    mov [screen_width], ax

    mov ax, [vbe_mode_info+0x14]    ; Высота экрана
    mov [screen_height], ax

    mov al, [vbe_mode_info+0x19]    ; Бит на пиксель
    mov [bpp], al

    ; === ПЕРЕХОД В PROTECTED MODE === ;
    ; Активация линии А20
    mov ax, 0x2401
    int 0x15

    ; Загрузка GDT
    cli
    lgdt [gdt_descriptor]

    ; Включение защищенного режима
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Переход в 32-битный режим и обновление CS
    jmp CODE_SEG:protected_mode

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

; Функция вывода числа в HEX (16-бит)
; Вход: DX = число для вывода
; Выход: -
print_hex_16:
    pusha               ; Сохраняем все регистры
    push ds
    push es
    xor bx, bx
    mov cx, 4           ; 4 ниббла
.next_nibble:
    rol dx, 4           ; Циклический сдвиг влево на 4 бита (чтобы обрабатывать старшие нибблы первыми)
    mov ax, dx          ; Копируем AX в DX
    and ax, 0x000F      ; Изолируем младшие 4 бита (текущий ниббл)
    add al, '0'         ; Преобразуем в ASCII (0-9)
    cmp al, '9'         ; Если значение > 9...
    jbe .print_char     
    add al, 7           ; ...добавляем 7 для букв A-F (ASCII 'A' - '9' = 8, но 0x0A -> 'A')
.print_char:
    mov ah, 0x0E        ; Функция BIOS: вывод символа
    mov bh, 0x00        ; Страница 0
    int 0x10            ; Выводим символ
    loop .next_nibble   ; Переходим к следующему нибблу
    mov ah, 0x0E        ; \r
    mov al, 0x0D
    mov bh, 0x00
    int 0x10
    mov ah, 0x0E        ; \n
    mov al, 0x0A
    mov bh, 0x00
    int 0x10
    pop es
    pop ds
    popa                ; Восстанавливаем регистры
    ret

; === Обработчики ошибок === ;
disk_error:
    mov si, error_msg
    call print_string
    cli
    hlt
vbe_read_error:
    mov si, vbe_read_error_msg
    call print_string
    cli
    hlt
vbe_set_error:
    mov si, vbe_set_error_msg
    call print_string
    cli
    hlt
loop_not_found:
    mov si, loop_not_found_msg
    call print_string
    cli
    hlt

; === Текстовые данные === ;
init_msg             db "Stage 2 entered", 0xD, 0xA, 0
disk_ok_msg          db "Kernel loaded from disk", 0xD, 0xA, 0
error_msg            db "Disk error on kernel loading!", 0xD, 0xA, 0
vbe_seg_msg          db "VBE offset and sector:", 0xD, 0xA, 0
vbe_read_error_msg   db "VBE not supported", 0xD, 0xA, 0
vbe_set_error_msg    db "Error setting VBE mode", 0xD, 0xA, 0
loop_not_found_msg   db "Video mode not found", 0xD, 0xA, 0
mode_found_msg       db "Mode found, it's number:", 0xD, 0xA, 0
fb_msg               db "Frame buffer address:", 0xD, 0xA, 0

; === Структуры для VBE === ;
vbe_info:       times 512 db 0
vbe_mode_info:  times 256 db 0

framebuffer     dd 0
screen_width    dw 0
screen_height   dw 0
bpp             db 0

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
    mov edi, [framebuffer]

    ; Установка цвета
    mov eax, 0x00ff0000     ; Красный (ARGB)

    ; Расчет общего кол-ва пикселей
    movzx ecx, word [screen_width]
    movzx ebx, word [screen_height]
    imul ecx, ebx

    ; Заливка фреймбуфера
    cld
    rep stosd

    ; Передача информации о VBE режме в ядро
    ; через регистры
    mov eax, [framebuffer]
    mov bx,  [screen_width]
    mov cx,  [screen_height]
    mov dl,  [bpp]

    ; Переход в ядро
    ; jmp 0x8600
