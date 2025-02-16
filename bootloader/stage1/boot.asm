[bits 16]
[org 0x7c00]

; === FAT12 header === ;
jmp short start
nop

bdb_oem:                    db 'mkfs.fat'
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1        ; 1 - MBR
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0x00E0   ; Максимум entries в root директории
bdb_total_sectors:          dw 2880     ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0xF0     ; 1.44 Floppy
bdb_sectors_per_fat:        dw 9        ; 9 секторов на FAT таблицу
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; === Extended boot record === ;
ebr_drive_number:           db 0x80                     ; 0x00 floppy, 0x80 hdd
                            db 0                        ; reserved
ebr_signature:              db 0x29
ebr_volume_id:              db 0x01, 0x02, 0x03, 0x04   ; serial number
ebr_volume_label:           db 'NONE OS    '            ; 11 bytes
ebr_system_id:              db 'FAT12   '               ; 8 bytes

start:
    ; Настройка сегментных регистров
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Настройка стека
    mov ss, ax
    mov sp, 0x7c00

    ; BIOS при загрузке устанавливает
    ; номер загрузочного диска в DL
    mov byte [ebr_drive_number], dl

    ; Установка текстового режима 80х25
    mov ax, 0x0003
    int 0x10

    ; === Чтение FAT таблиц и Root directory === ;
    cli
    mov ax, 31          ; Количество секторов для чтения ( 32 - 1MBR )
    mov bx, 0x7e00      ; Адрес, куда загружать ( 0x7C00 + 512 )
    mov edx, 1
    call read_lba
    sti
    jc disk_error       ; Проверка ошибки (CF=1 при ошибке)
    ; При отсутствии ошибки, вывести соотв. сообщение
    mov si, disk_ok_msg
    call print_string

    ; Получения адреса начала root dir
    mov si, 0x7e00      ; Начало FAT таблиц

    ; Смещение SI до начала root dir
    xor dx, dx
    mov ax, word [bdb_bytes_per_sector]
    mov bx, word [bdb_sectors_per_fat]
    mov cx, word [bdb_fat_count]
    mul bx  ; AX = bytes_per_sector * sectors_per_fat
    mul cx  ; AX = bytes_per_sector * sectors_per_fat * fat_count

    add si, ax   ; SI = начало root dir

.stage2_loop:
    ; Сравнить имя с 'STAGE2  BIN'
    mov di, stage2_name
    mov cx, 11          ; 11 байт имени
    push si
    repe cmpsb          ; Побайтовое сравнение SI и DI
    pop si

    jz .stage2_found
    add si, 32          ; Если не STAGE2, переход к следующему файлу
    jmp .stage2_loop

.stage2_found:
    mov bx, data_start  ; Адрес загрузки следующего сектора
    mov di, [si + 26]   ; Номер первого кластера с файлом
    mov cx, 11          ; Проверка имени файла
    call print_string_count
    
.read_loop:
    ; Чтение следующего сектора файла
    mov dx, di                           ; DX = cluster
    sub dx, 2                            ; DX = cluster - 2
    mul byte [bdb_sectors_per_cluster]   ; DX = (cluster-2)*sec_per_cluster
    add dx, 33                           ; DX = start+(cluster-2)*sec_per_cluster

    cli
    mov ax, 1           ; Количество секторов для чтения
    call read_lba
    sti
    jnc .read_ok         ; Проверка ошибки (CF=1 при ошибке)
    mov si, loop_read_error_msg
    call print_string
.read_ok:
    ; При отсутствии ошибки, вывести соотв. сообщение
    mov si, chain_disk_ok_msg
    call print_string

    ; Получение адреса следующего кластера в FAT таблице
    push bx
    mov ax, di  ; ax = di
    mov bx, 3   ; ax = di * 3
    mul bx

    mov bx, 2
    div bx      ; ax = di * 3 / 2
    pop bx

    mov di, fat_start       ; DI = FAT_start
    add di, ax              ; DI = FAT_start + offset = next_cluster_ptr
    mov di, [di]            ; DI = next_cluster number

    ; Поскольку номера 12-битные, нужны преобразования
    ; Например следующий номер в памяти хранится как 12 34
    ; Учитывая, что данные хранятся в little-endian,
    ;   Номер ячейки четный:
    ;       12 34 -> 0x3412 -> and 0x0FFF -> 0x0412
    ;   Номер нечетный:
    ;       12 34 -> 0x3412 -> shr 4 -> 0x0341
    or dx, dx               ; Проверка на четность после деления на 2 (dx=di*3 % 2)

    jz .even

.odd:
    shr di, 4
    jmp .after_check

.even:
    and di, 0x0FFF

.after_check:
    cmp di, 0xFF8   ; di > 0xFF8 -> конец файла
    jae .read_end

    add bx, 512     ; Смещение адреса записи на 512 ( 1 сектор )
    jmp .read_loop

.read_end:
    mov si, stage2_loaded_msg
    call print_string

    jmp 0x0000:data_start

; Функция read_lba: читает секторы с диска через LBA
; Вход:
;   ES:BX = Адрес буфера (сегмент:смещение)
;   AX    = Количество секторов для чтения (1-255)
;   EDX   = LBA-сектор (32-битное значение)
; Выход:
;   CF = 1 при ошибке, 0 при успехе
;   AX = Код ошибки (если CF=1)
read_lba:
    pusha               ; Сохраняем все регистры
    push es            ; Сохраняем ES
    mov di, sp         ; DI указывает на вершину стека

    ; dap_size:                   db 0x10       ; Размер DAP
    ; dap_reserved:               db 0x00       ; Зарезервировано
    ; dap_num_sectors:            dw 1          ; Секторов для чтения
    ; dap_offset:                 dw 0x7E00     ; Смещение буфера
    ; dap_segment:                dw 0x0000     ; Сегмент буфера
    ; dap_lba:                    dq 0

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
    mov dl, [ebr_drive_number] ; Номер диска (переданный BIOS)
    int 0x13           ; Вызов прерывания
    jc .error          ; Если ошибка (CF=1)

    ; Успех: очищаем стек и возвращаем CF=0
    add sp, 16         ; Удаляем DAP из стека
    pop es             ; Восстанавливаем ES
    popa               ; Восстанавливаем все регистры
    clc                ; CF = 0
    ret
.error:
    mov [di + 18], ax  ; Сохраняем код ошибки в AX (через стек)
    add sp, 16         ; Удаляем DAP из стека
    pop es             ; Восстанавливаем ES
    popa               ; Восстанавливаем регистры
    stc                ; CF = 1
    ret

; Обработчик ошибки чтения с диска
disk_error:
    mov si, error_msg
    call print_string
    cli
    hlt

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
    mov ax, 0x0E0D        ; \r
    mov bh, 0x00
    int 0x10
    mov ax, 0x0E0A        ; \n
    int 0x10
    popa
    ret

; Данные
disk_ok_msg         db "FAT table and root loaded", 0xD, 0xA, 0
chain_disk_ok_msg   db "Next cluster loaded", 0xD, 0xA, 0
stage2_loaded_msg   db "Stage2 loaded into memory", 0xD, 0xA, 0
loop_read_error_msg db "Err FAT chain", 0xD, 0xA, 0
error_msg           db "Disk error on loading stage2!", 0xD, 0xA, 0
stage2_name         db "STAGE2  BIN"
fat_start           equ 0x7E00  ; Начало FAT (0x7C00 + 512)
data_start          equ 0xBC00  ; Начало данных (0x7C00 + 512*32)

; Заполнение до 512 байт
times 510 - ($ - $$) db 0
dw 0xaa55
