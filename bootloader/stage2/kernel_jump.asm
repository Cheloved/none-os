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
    ; jmp 0x08:protected_mode
    push word [CODE_SEG]
    push word protected_mode
    retf


[bits 32]
protected_mode:
    ; Обновление сегментных регистров
    ; mov ax, 0x10
    mov ax, [DATA_SEG]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Настройка 32-битного стека
    ; mov esp, 0x10000
    mov esp, 0x7C00

    mov eax, Multiboot
    mov ebx, info
    mov edx, 0x2BADB002

    ; Переход к загруженному ядру
    ; jmp 0x08:0x10000
    push dword [CODE_SEG]
    push dword 0x10000 
    retf
