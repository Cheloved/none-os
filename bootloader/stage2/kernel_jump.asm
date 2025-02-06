[bits 16]

extern Multiboot
extern info
extern current_gdt_desc
extern CODE_SEG
extern DATA_SEG
global kernel_jump

kernel_jump:
     ; === ПЕРЕХОД В PROTECTED MODE === ;
    ; Активация линии А20
    mov ax, 0x2401
    int 0x15

    ; Загрузка GDT
    cli
    lgdt [current_gdt_desc]

    ; Включение защищенного режима
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Переход в 32-битный режим и обновление CS
    jmp 0x08:protected_mode


[bits 32]
protected_mode:
    ; Обновление сегментных регистров
    ; mov ax, DATA_SEG
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Настройка 32-битного стека
    mov esp, 0x10000

    ; mov edi, 0xfd000020
    ; mov dword [edi], 0x0000FF00

    mov eax, Multiboot
    mov ebx, info
    mov edx, 0x2BADB002

    ; Переход к загруженному ядру
    jmp 0x08:0x10000
