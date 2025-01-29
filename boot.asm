[bits 16]
[org 0x7c00]

; === FAT12 header ===
jmp short start
nop

bdb_oem:                    db 'mkfs.fat'
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1        ; 1 - MBR, 2-5 - stage2, 
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
ebr_drive_number:           db 0                        ; 0x00 floppy, 0x80 hdd
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

    ; Установка текстового режима 80х25
    mov ax, 0x0003
    int 0x10

    ; === Чтение FAT таблиц и Root directory === ;
    ; Получение адреса сектора в формате CHS
    mov ax, 1        ; LBA = 1
    call lba2chs

    cli
    mov ah, 0x02        ; Функция чтения секторов
    mov al, 31          ; Количество секторов для чтения
    mov bx, 0x7e00      ; Адрес, куда загружать
    int 0x13
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
    repe cmpsb          ; Побайтовое сравнение SI и DI

    jz .stage2_found
    add si, 31          ; Если не STAGE2, переход к следующему файлу
    jmp .stage2_loop

.stage2_found:
    mov cx, 0x7e00      ; Начало FAT таблиц
    mov dx, 0xBC00      ; Начало части с данными
    mov bx, dx          ; Адрес загрузки следующего сектора

    mov di, [si + 26]   ; Номер первого кластера с файлом
    
.read_loop:
    ; Чтение следующего сектора файла
    ; Получение адреса сектора в формате CHS
    mov ax, di                              ; AX = cluster
    sub ax, 2                               ; AX = cluster - 2
    mul byte [bdb_sectors_per_cluster]      ; AX = (cluster-2)*sec_per_cluster
    add ax, word [data_start]               ; AX = start+(cluster-2)*sec_per_cluster
    call lba2chs

    cli
    mov ah, 0x02        ; Функция чтения секторов
    mov al, 1           ; Количество секторов для чтения
    int 0x13
    sti

    ; Получение адреса следующего кластера в FAT таблице
    push bx
    mov ax, di  ; ax = di
    mov bx, 3   ; ax = di * 3
    mul bx

    mov bx, 2
    div bx      ; ax = di * 3 / 2
    add ax, dx  ; ax = (di * 3 / 2) + (di*3/2) % 2
    pop bx

    mov di, [fat_start]
    add di, ax     ; di = адрес следующего кластера
    mov ax, [di]
    mov di, ax
    shr di, 4      ; di = номер следующего кластера

    cmp di, 0xFF8
    jae .read_end

    add bx, 512
    jmp .read_loop
.read_end:
    mov si, stage2_loaded_msg
    call print_string

    jmp 0xBE00

fat_read_error:
    mov si, fat_read_error_msg
    call print_string
    cli
    hlt

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

; Функция перевода LBA(Logical Block Address) в CHS(Cylinder-Head-Sector)
; Вход:
;   - ax:             LBA адрес
; Выход:
;   - cx[bits 0-5] :  сектор
;   - cx[bits 6-15]:  цилиндр
;   - dh           :  головка
lba2chs:
    push ax
    push dx

    xor dx, dx                              ; dx = 0
    div word [bdb_sectors_per_track]      ; ax = LBA / sectors_per_track
    ; div word [0x7C00 + 0x18]                ; ax = LBA / sectors_per_track
                                            ; dx = LBA % sectors_per_track
    inc dx                                  ; dx = (LBA % sectors_per_track + 1) = sector
    mov cx, dx                              ; cx = sector

    xor dx, dx                              ; dx = 0
    div word [bdb_heads]                  ; ax = (LBA/sectors_per_track)/heads = cylinder
    ; div word [0x7C00 + 0x1A]                ; ax = (LBA/sectors_per_track)/heads = cylinder
                                            ; dx = (LBA/sectors_per_track)%heads = head

    mov dh, dl                              ; dh = head
    mov ch, al                              ; ch = cylinder (lower 8 bits)
    shl ah, 6
    or cl, ah                               ; cl[6-8] = cylinder (upper 2 bits)

    pop ax
    mov dl, al                              ; Восстановить DL
    pop ax

    ret

; Данные
disk_ok_msg       db "FAT table and root dir loaded from disk", 0xD, 0xA, 0
stage2_loaded_msg db "Stage2 loaded into memory", 0xD, 0xA, 0
before_stage2_msg db "Entering stage 2", 0xD, 0xA, 0
fat_read_error_msg db "Error reading FAT table", 0xD, 0xA, 0
error_msg         db "Disk error on loading stage2!", 0xD, 0xA, 0
stage2_name       db "STAGE2  BIN"
fat_start         dw 0x7E00
data_start        dw 0xBE00

; Заполнение до 512 байт
times 510 - ($ - $$) db 0
dw 0xaa55
