[bits 16]

extern selected_mode

extern current_gdt
extern current_gdt_desc
extern CODE_SEG
extern DATA_SEG

extern disable_a20
extern load_gdt
extern goto_protected

; Активация линии А20
disable_a20;
    mov ax, 0x2401
    int 0x15
    ret

; Загрузка GDT
load_gdt:
    cli
    lgdt [current_gdt_desc]

    ret

; Включение защищенного режима
goto_protected:
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Переход в 32-битный режим и обновление CS
    jmp CODE_SEG:protected_mode

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

